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
#include "FFT.h"

extern MCBSP_Handle aicMcbsp;

// Pulled from myfir
extern int16_t lpfCoeff[];
extern int16_t hpfCoeff[];

const int16_t* filter_coeff;
static int16_t filter_length = 0;
static int16_t leftRightFlag = 0;
static uint16_t muteFlag = 0;
int16_t delayLineL[LPF_COEFF_LEN], delayLineR[LPF_COEFF_LEN];

uint16_t normalize(int16_t value);

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
    FFT_initialize();
    sample_t sample = {LEFT, -1};
    uint16_t i = 0;
    // Buffer Audio
    for(i=0; i<48; i++)
        q_push(sample);
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
 * BIOS Task to process 256 samples of filtered
 * data from the AudioProcessing task within an
 * FFT function.
 *
 * FFT Function implemented using Simulink Code Gen.
 *
 * This task also generates a frame to be displayed
 * on the LCD which is pushed to the "Display Manager"
 * task.
 */
void TSK_FFT()
{
    uint16_t i = 0, j = 0, shift = 0;
    uint16_t val, low, high;
    uint16_t fb[96];

    while(1)
    {
        MBX_pend(&mbx_processedAudio, FFT_U.In1, SYS_FOREVER);
        FFT_step();
        j = 0;
        shift=0;
        for(i=0; i<32; i++)
        {
            val = normalize(FFT_Y.Out1[i]);
            low = 0x00ff & val;
            high = 0x00ff & (val >> 8);

            if (shift == 1)
            {
                // Upper half
                fb[j] = (high << 8);
                fb[j+1] = high;
                // Lower half
                fb[j+48] = (low << 8);
                fb[j+49] = low;
                shift = 0;
                j+=2;
            }
            else
            {
                // Upper half
                fb[j] = (high << 8)| high;
                fb[j+1] = 0x0000;
                // Lower half
                fb[j+48] = (low << 8) | low;
                fb[j+49] = 0x0000;
                shift = 1;
                j++;
            }
        }
        if(!MBX_post(&mbx_frameBuffer, fb, 0))
            LOG_printf(&trace, "TSK_FFT: Failed to post to 'mbx_frameBuffer'");
    }
}

/**
 * "Normalizes" to a display value of 0-16 such that values above
 * 2000 are capped at 16 and signals are linearly broken up at
 * increments of 125.
 */
uint16_t normalize(int16_t value)
{
    if(value >= 2000)
    {
        return 0xffff;
    }
    else if(value >= 1875)
    {
        return 0xfeff;
    }
    else if(value >= 1750)
    {
        return 0xfcff;
    }
    else if(value >= 1625)
    {
        return 0xf8ff;
    }
    else if(value >= 1500)
    {
        return 0xf0ff;
    }
    else if(value >= 1375)
    {
        return 0xe0ff;
    }
    else if(value >= 1250)
    {
        return 0xc0ff;
    }
    else if(value >= 1125)
    {
        return 0x80ff;
    }
    else if(value >= 1000)
    {
        return 0x00ff;
    }
    else if(value >= 875)
    {
        return 0x00fe;
    }
    else if(value >= 750)
    {
        return 0x00fc;
    }
    else if(value >= 625)
    {
        return 0x00f8;
    }
    else if(value >= 500)
    {
        return 0x00f0;
    }
    else if(value >= 375)
    {
        return 0x00e0;
    }
    else if(value >= 250)
    {
        return 0x00c0;
    }
    else if(value >= 125)
    {
        return 0x0080;
    }
    return 0x0000;
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
    command_t command = TOGGLE_MUTE;
    uint16_t i, j = 0;
    Uns old;
    sample_t samples[MBX_AUDIO_LEN];
    int16_t samplesForFFT[256];
    while(1)
    {
        // wait for data
        MBX_pend(&mbx_audio, samples, SYS_FOREVER);
        // Pull a command off of the buffer, only process if exists
        if(MBX_pend(&mbx_command, &command, 0))
        {
            if(command == TOGGLE_MUTE)
                ToggleMute();
            else
                SetFilter(command);
        }
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
            if (processedSample.channel == LEFT)
            {
                samplesForFFT[j] = processedSample.data;
                j++;
                if (j >= 256)
                {
                    j = 0;
                    if(!MBX_post(&mbx_processedAudio, samplesForFFT, 0))
                        LOG_printf(&trace, "TSK_AudioProcessing: Failed to post to 'mbx_processedAudio'");
                }
            }
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
        idx = 0;
        if(!MBX_post(&mbx_audio, samples, 0))
            LOG_printf(&trace, "HWI_I2S_Rx: Failed to post to 'mbx_audio'");
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
