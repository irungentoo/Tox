/* AV_codec.h
 *
 * Audio and video codec intitialisation, encoding/decoding and playback
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of Tox.
 *
 *  Tox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tox.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*----------------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/opt.h>
#include <pthread.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <SDL.h>


/* ffmpeg VP8 codec ID */
#define VIDEO_CODEC 		AV_CODEC_ID_VP8

/* ffmpeg Opus codec ID */
#define AUDIO_CODEC 		AV_CODEC_ID_OPUS

/* default video bitrate in bytes/s */
#define VIDEO_BITRATE 	200*1000

/* default audio bitrate in bytes/s */
#define AUDIO_BITRATE	64000

/* audio frame duration in miliseconds */
#define AUDIO_FRAME_DURATION	20

/* audio sample rate recommended to be 48kHz for Opus */
#define AUDIO_SAMPLE_RATE	48000

/* the amount of samples in one audio frame */
#define AUDIO_FRAME_SIZE	AUDIO_SAMPLE_RATE*AUDIO_FRAME_DURATION/1000

/* the quit event for SDL */
#define FF_QUIT_EVENT (SDL_USEREVENT + 2)

#ifdef __linux__
#define VIDEO_DRIVER "video4linux2"
#define DEFAULT_WEBCAM "/dev/video0"
#define AUDIO_DRIVER "alsa"
#define DEFAULT_AUDIO_DEVICE "default"
#endif

#ifdef WIN32
#define VIDEO_DRIVER "vfwcap"
#define DEFAULT_WEBCAM "0"
#define AUDIO_DRIVER ""
#define DEFAULT_AUDIO_DEVICE ""
#endif

SDL_Surface     *screen;



typedef struct
{
    SDL_Overlay *bmp;
    int width, height;
} VideoPicture;


typedef struct
{

    uint8_t send_audio;
    uint8_t receive_audio;
    uint8_t send_video;
    uint8_t receive_video;

    uint8_t support_send_audio;
    uint8_t support_send_video;
    uint8_t support_receive_audio;
    uint8_t support_receive_video;


    AVInputFormat   	*video_input_format;
    AVFormatContext 	*video_format_ctx;
    uint8_t             	 video_stream;

    AVInputFormat   	*audio_input_format;
    AVFormatContext 	*audio_format_ctx;
    uint8_t              	audio_stream;

    AVCodecContext  	*webcam_decoder_ctx;
    AVCodec         	*webcam_decoder;

    AVCodecContext  	*video_encoder_ctx;
    AVCodec         	*video_encoder;

    AVCodecContext  	*video_decoder_ctx;
    AVCodec         	*video_decoder;

    AVCodecContext  	*microphone_decoder_ctx;
    AVCodec         	*microphone_decoder;

    AVCodecContext  	*audio_encoder_ctx;
    AVCodec         	*audio_encoder;

    AVCodecContext  	*audio_decoder_ctx;
    AVCodec         	*audio_decoder;

    AVFrame         	*enc_audio_frame;
    AVFrame         	*audio_frame;
    AVFrame         	*microphone_frame;

    AVFrame         	*webcam_frame;
    AVFrame         	*s_video_frame;
    AVFrame         	*r_video_frame;

    AVPacket        	enc_video_packet;
    AVPacket        	enc_audio_packet;

    AVPacket        	dec_video_packet;
    AVPacket        	dec_audio_packet;

    uint8_t req_video_refresh;


    int             	audio_frame_finished;
    int 		video_frame_finished;
    int 		dec_frame_finished;

    /* context for converting image format to something SDL can use*/
    struct SwsContext 	*sws_SDL_r_ctx;

    /* context for converting webcam image format to something the video encoder can use */
    struct SwsContext 	*sws_ctx;

    /* rendered video picture, ready for display */
    VideoPicture	video_picture;

    //SDL_Thread      	*parse_tid;
    rtp_msg_t*     	s_video_msg;
    rtp_msg_t*     	s_audio_msg;
    rtp_msg_t*     	r_msg;
    rtp_session_t* 	_m_session;
    int socket;
    
    uint8_t             quit;
    uint8_t 		SDL_initialised;
    SDL_Event       SDL_event;

    pthread_t encode_audio_thread;
    pthread_t encode_video_thread;
    pthread_t decode_thread;
    pthread_mutex_t rtp_msg_mutex_lock;

    ALCdevice *dev;
    ALCcontext *ctx;
    ALuint source, *buffers;
    ALuint buf;
    ALint val;


} codec_state;

int display_received_frame(codec_state *cs);
int decode_video_frame(codec_state *cs);
int encode_audio_frame(codec_state *cs);
int init_receive_audio(codec_state *cs);
int init_decoder(codec_state *cs);
int init_send_video(codec_state *cs);
int init_send_audio(codec_state *cs);
int init_encoder(codec_state *cs);
int video_encoder_refresh(codec_state *cs, int bps);
void *encode_video_thread(void *arg);
void *encode_audio_thread(void *arg);
int video_decoder_refresh(codec_state *cs, int width, int height);
int decoder_handle_rtp_packet(codec_state *cs);
void *decode_thread(void *arg);
