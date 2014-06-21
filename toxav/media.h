/**  media.h
 *
 *   Audio and video codec intitialization, encoding/decoding and playback
 *
 *   Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *   This file is part of Tox.
 *
 *   Tox is open source software: you can redistribute it and/or modify
 *   it under the terms of the StopNerds Public License as published by
 *   the StopNerds Foundation, either version 1 of the License, or
 *   (at your option) any later version.
 *
 *   Tox is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   StopNerds Public License for more details.
 *
 *   You should have received a copy of the StopNerds Public License
 *   along with Tox. If not, see <http://stopnerds.org/license/>.
 *
 */

#ifndef _AVCODEC_H_
#define _AVCODEC_H_

#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include <vpx/vpx_decoder.h>
#include <vpx/vpx_encoder.h>
#include <vpx/vp8dx.h>
#include <vpx/vp8cx.h>
#define VIDEO_CODEC_DECODER_INTERFACE (vpx_codec_vp8_dx())
#define VIDEO_CODEC_ENCODER_INTERFACE (vpx_codec_vp8_cx())

/* Audio encoding/decoding */
#include <opus.h>

typedef enum _Capabilities {
    none,
    a_encoding = 1 << 0,
    a_decoding = 1 << 1,
    v_encoding = 1 << 2,
    v_decoding = 1 << 3
} Capabilities;

typedef struct _CodecState {

    /* video encoding */
    vpx_codec_ctx_t  v_encoder;
    uint32_t frame_counter;

    /* video decoding */
    vpx_codec_ctx_t  v_decoder;

    /* audio encoding */
    OpusEncoder *audio_encoder;
    int audio_bitrate;
    int audio_sample_rate;

    /* audio decoding */
    OpusDecoder *audio_decoder;

    uint64_t capabilities; /* supports*/

} CodecState;


typedef struct _JitterBuffer {
    RTPMessage **queue;
    uint16_t capacity;
    uint16_t size;
    uint16_t front;
    uint16_t rear;
    uint8_t queue_ready;
    uint16_t current_id;
    uint32_t current_ts;
    uint8_t id_set;
} JitterBuffer;

JitterBuffer *create_queue(int capacity);
void terminate_queue(JitterBuffer *q);
void queue(JitterBuffer *q, RTPMessage *pk);
RTPMessage *dequeue(JitterBuffer *q, int *success);


CodecState *codec_init_session ( uint32_t audio_bitrate,
                                 uint16_t audio_frame_duration,
                                 uint32_t audio_sample_rate,
                                 uint32_t audio_channels,
                                 uint16_t video_width,
                                 uint16_t video_height,
                                 uint32_t video_bitrate );

void codec_terminate_session(CodecState *cs);

#endif
