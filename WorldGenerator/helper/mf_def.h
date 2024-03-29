#ifndef MF_DEF_H
#define MF_DEF_H

#define TASK_GENERATE_MAP 0x0001
#define TASK_SAVE_MAP 0x0002
#define TASK_VIEW_MAP 0x0003
#define TASK_IDLE 0xFFFF

#define MAX_LACUNARITY 9.0
#define MIN_LACUNARITY 0.5

#define INDEX_GENERATOR 0
#define INDEX_COMBINER 1
#define INDEX_MODIFIER 2
#define INDEX_SELECTOR 3
#define INDEX_TRANSFORMER 4

#define INDEX_ABS 0
#define INDEX_CLAMB 1
#define INDEX_CURVE 2
#define INDEX_EXPONENT 3
#define INDEX_INVERT 4
#define INDEX_SCALEBIAS 5
#define INDEX_TERRACE 6

#define INDEX_DISPLACEMENT 0
#define INDEX_ROTATEPOINT 1
#define INDEX_SCALEPOINT 2
#define INDEX_TRANSLATEPOINT 3
#define INDEX_TURBULENCE 4

#define INDEX_BLEND 0
#define INDEX_SELECT 1

#define INDEX_BILLOW 0
#define INDEX_CHESS 1
#define INDEX_CONST 2
#define INDEX_CYLINDERS 3
#define INDEX_PERLIN 4
#define INDEX_RIDGED 5
#define INDEX_SPHERES 6
#define INDEX_VORONI 7

#endif // MF_DEF_H
