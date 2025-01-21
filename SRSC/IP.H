/****************************************************************************
 * file - ip.h                                                              *
 ****************************************************************************/

/* typedefs */

typedef unsigned char *image_ptr;
typedef double *double_ptr;
typedef struct
    {
    unsigned char r,g,b;        // 이미지의 색을 red, green, blue로 나눠서 추출
    } pixel;

typedef pixel *pixel_ptr;       // 이미지의 픽셀 포인터

typedef struct
    {
    int width;      //메쉬 너비
    int height;     // 메쉬 높이
    float *x_data;  // 메쉬의 가로 데이터
    float *y_data;  // 메쉬의 세로 데이터
    } mesh;

typedef struct
    {
    double re;      // 복소수의 실수
    double im;      // 복소수의 허수
    } COMPLEX;

typedef COMPLEX *complex_ptr;       // 복소수 포인터

typedef struct
    {
    int x;      // x 좌표
    int y;      // y 좌표
    } POINT;

typedef struct 
    {
    POINT P;       // 선의 점
    POINT Q;        // 선의 점
    int dx, dy;     // 미분 값
    float length;       //선의 길이
    long length_squared;        //선의 길이 제곱
    } LINE;

typedef struct 
    {
    POINT P;        // 선분의 시작점
    POINT Q;        // 선분의 끝점
    } LINE_SEGMENT;

typedef struct 
    {
    int number;             /* number of segments to follow */
    LINE_SEGMENT line[100];
    char *filename; /* name of file holding the line list */
    } LINE_LIST;

/* defines */

#define PI   3.14159265358979323846
#define CLIP(val, low, high) {if(val<low) val=low; if(val>high) val=high;}
#define CLAMP(val, low, high) ((val<low) ? low : ((val>high) ? high : val))
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#define IP_MALLOC(X) malloc(X)
#define IP_FREE(X) free(X)
#define PBM 4
#define PGM 5
#define PPM 6

