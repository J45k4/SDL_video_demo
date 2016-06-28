extern "C" {
	#include <libavcodec\avcodec.h>
	#include <libavformat\avformat.h>
	#include <libswscale\swscale.h>
}


void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);