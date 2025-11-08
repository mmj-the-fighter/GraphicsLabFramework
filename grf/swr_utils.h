#ifndef _SWR_UTILS_H_
#define _SWR_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct _swr_rect{
		int left;
		int top;
		int width;
		int height;
	}swr_rect;
	int swr_get_max(int a, int b);
	int swr_get_min(int a, int b);
	void swr_intersect_rect_rect(const swr_rect *r1, const swr_rect* r2, swr_rect* ir);
#ifdef __cplusplus
}
#endif

#endif