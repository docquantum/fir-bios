/**
 * Audio processing library which makes heavy use of BIOS mechanics.
 *
 * TCONF Defined variables:
 *  - que_processedAudio: Atomic queue to hold processed samples (circular buffer)
 *  - mbx_audio: Mailbox to hold 1msec of audio during receive process.
 *  - mbx_messages: Mailbox to hold messages to control the processing
 */



#include <std.h>

#include <log.h>

#include "stdio.h"
#include "bioscfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "string.h"
#include "myfir.h"
#include "custom_types.h"
#include "queue.h"

extern MCBSP_Handle aicMcbsp;

// Pulled from myfir
extern int16_t lpfCoeff[];
extern int16_t hpfCoeff[];

static int16_t* filter_coeff;
static int16_t filter_length = 0;
static int16_t leftRightFlag = 0;
static uint16_t muteFlag = 0;
static int16_t delayLineL[LPF_COEFF_LEN], delayLineR[LPF_COEFF_LEN];

/*
 * Toggles the mute flag
 */
void ToggleMute()
{
    muteFlag = muteFlag ? 0 : 1;
}

/*
 * Returns the mute flag state
 */
uint16_t GetMute()
{
    return muteFlag;
}

/*
 * Initializes the audio processing task variables
 */
void audioProcessingInit(void)
{
    memset(delayLineL, 0, LPF_COEFF_LEN);
    memset(delayLineR, 0, LPF_COEFF_LEN);
}

/*
 * Set's the currently used filter pointers
 */
void SetFilter(command_t type)
{
    switch(type)
    {
        case FILTER_NONE:
            filter_coeff = NULL;
            filter_length = 0;
            break;
        case FILTER_HPF:
            filter_coeff = hpfCoeff;
            filter_length = HPF_COEFF_LEN;
            break;
        case FILTER_LPF:
            filter_coeff = lpfCoeff;
            filter_length = LPF_COEFF_LEN;
            break;
        default:
            filter_coeff = NULL;
            filter_length = 0;
            break;
    }
}

/*
 * BIOS Task to process incoming audio data
 * Receives 1 msec of audio from the receiver thread
 * to then filter based on the system state.
 *
 * System state is modified by the command mailbox
 * which is posted by the userInterface task.
 */
void TSK_AudioProcessing()
{
    sample_t processedSample = {LEFT, 0};
    command_t state = 0;
    uint16_t i = 0;
    Uns old;
    sample_t samples[MBX_AUDIO_LEN];
    while(1)
    {
        // wait for data
        MBX_pend(&mbx_audio, samples, SYS_FOREVER);
        // Pull a command off of the buffer, only process if exists
        if(MBX_pend(&mbx_command, &state, 0))
        {
            if(state == TOGGLE_MUTE)
                ToggleMute();
            else
                SetFilter(state);
        }
        else
            LOG_printf(&trace, "HWI_I2S_Rx: Failed to post to 'mbx_audio'");
        for(i = 0; i < MBX_AUDIO_LEN; i++)
        {
            if (muteFlag)
            {
                processedSample.data = 0;
            }
            else
            {
                myfir((const int16_t *) &samples[i].data,
                      filter_coeff,
                      &processedSample.data,
                      samples[i].channel ? delayLineL : delayLineR,
                      1,
                      filter_length
                );
            }
            processedSample.channel = samples[i].channel;
            old = HWI_disable();
            q_push(processedSample);
            HWI_restore(old);
        }
    }
}

/**
 * BIOS HWI Thread which triggers when audio codec
 * has a new sample on its registers to read.
 *
 * Reads in 1msec of audio before posting it to a mailbox
 */
void HWI_I2S_Rx(void)
{
    static sample_t samples[MBX_AUDIO_LEN];
    static uint16_t idx = 0;
    if (leftRightFlag == 0)
    {
        samples[idx].data = MCBSP_read16(aicMcbsp);
        samples[idx].channel = LEFT;
        leftRightFlag = 1;
        idx++;
    }
    else
    {
        samples[idx].data = MCBSP_read16(aicMcbsp);
        samples[idx].channel = RIGHT;
        leftRightFlag = 0;
        idx++;
    }
    if(idx >= MBX_AUDIO_LEN)
    {
        if(!MBX_post(&mbx_audio, samples, 0))
            LOG_printf(&trace, "HWI_I2S_Rx: Failed to post to 'mbx_audio'");
        idx = 0;
    }

}

/**
 * BIOS HWI Thread which triggers when audio codec
 * is ready to read a new sample.
 *
 * Pulls a single sample at a time from a queue
 */
void HWI_I2S_Tx(void)
{
    static sample_t sample = {LEFT, 0};
    sample = q_pop();
    MCBSP_write16(aicMcbsp, sample.data);
}
