#include <iostream>
#include <stdlib.h>
#include "SaveFrame.h"
extern "C" {
	#include <libavcodec\avcodec.h>
	#include <libavformat\avformat.h>
	#include <libswscale\swscale.h>
}
using namespace std;

int main(int argc, char *argv[]) {
	av_register_all();
	cout << "Tulostaa" << endl;
	
	AVFormatContext *pFormatCtx = NULL;
	if (avformat_open_input(&pFormatCtx, "c:\\videoo\\GOPR1510.MP4", NULL, 0) != 0) {
		cout << "opening failed" << endl;
		system("pause");
		return -1;
	}

	cout << "Viela kaynnissa" << endl;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		cout << "no stream info" << endl;
		system("pause");
		return -1;
	}

	//av_dump_format(pFormatCtx, 0, "c:\\viteoo\\GOPR1510.MP4", 0);

	int i;

	AVCodecContext *pCodecCtxOrig = NULL;
	AVCodecContext *pCodecCtx = NULL;

	int videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1) {
		cout << "VideoStreamia ei loytynyt" << endl;
		system("pause");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	AVCodec * pCodec = NULL;

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		cout << "Kodekkia ei ole tuettu" << endl;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
		cout << "Codec kopiointi epaonnistui" << endl;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		return -1;
	}

	AVFrame *pFrame = NULL;
	AVFrame *pFrameRGB = NULL;

	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();

	uint8_t *buffer = NULL;
	int numBytes;
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height);

	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
		pCodecCtx->width, pCodecCtx->height);


	// Reading

	struct SwsContext *sws_ctx = NULL;
	int frameFinished;
	AVPacket packet;
	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		AV_PIX_FMT_RGB24,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL);

	i = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if (frameFinished) {
				sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);

				if (++i <= 5) {
					SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
				}
			}
			av_free_packet(&packet);
		}
	}

	system("pause");

	return 0;
}