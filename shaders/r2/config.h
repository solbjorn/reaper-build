/*
    ECB-shaderpack configuration file
    ______________________________________
*/

#ifndef CONFIG_H
#define CONFIG_H

// Motion Blur and Bump
#define ECB_MBUMP                                   // микроструктурный бамп

#define ECB_DOF                                     // Depth of Field
    #define ECB_DOF_MINDIST 0.5f                    // минимальное расстояние для эффекта
    #define ECB_DOF_MAXDIST 600.f                   // максимальное расстояние для эффекта
    #define ECB_DOF_MAXCOF 4.f                      // максимальный фактор размытия удаленных объектов
    #define ECB_DOF_MAXCOF_NEAR 4.f                 // максимальный фактор размытия близких объектов
    #define ECB_DOF_MAXNEAR float(0.7)              // расстояние до близкого кольца DoF с минимальным размытием
    #define ECB_DOF_MINNEAR float(0.0)              // расстояние до окончания близкого кольца DoF с максимальным размытием

#define ECB_DDOF                                    // Dynamic Depth of Field
    #define ECB_DDOF_MINDIST_ADD float(0.7)         // расстояние старта размытия
    #define ECB_DDOF_MAXDIST_FACTOR float(4.0)      // домножение величины размытия удаленных объектов при взгляде на близкий
    #define ECB_DDOF_MINDIST float(0.0)             // минимальное расстояние применения эффекта
    #define ECB_DDOF_MAXDIST float(10.0)            // максимальное расстояние применения эффекта
    #define ECB_DDOF_AIM                            // Использовать размывку при прицеливании
    #define ECB_DDOF_AIM_BLUR 7.5f                  // Размытие в режиме прицеливания
    #define ECB_DDOF_AIM_CIRCLE 1.75f               // Размер неразмытого круга в центре экрана

#define ECB_SSAO                                    // Screen Space Ambient Occlusion

#define OGSE_GODRAYS
    #define OGSE_GODRAYS_FARPLANE float(180.0)
    // качество саншафтов включается в консоли: r2_sun_shafts. 0 - выкл, 1 - низкое качество (20 семплов), 2 - среднее качество (50 семплов), 3 - высокое качество (100 семплов). Конкретные значения настраивайте в godRays.h
    #define OGSE_GODRAYS_DENSITY float(0.7)          // "плотность" лучей, чем меньше, тем они короче, но тем выше качество
    #define OGSE_GODRAYS_BLEND_FACTOR float(0.0)     // фактор смешивания с рассеянным светом. Чем меньше значение, тем меньше "засветка" от эффекта, но теряется "объемность"
    #define OGSE_GODRAYS_DUST                        // включить пыль в саншафтах
    #define OGSE_GODRAYS_DUST_SPEED float(0.4)       // скорость частиц пыли
    #define OGSE_GODRAYS_DUST_INTENSITY float(9.0)   // яркость пылинок
    #define OGSE_GODRAYS_DUST_DENSITY float(1.0)     // плотность частиц пыли
    #define OGSE_GODRAYS_DUST_SIZE float(0.7)        // размер пылинок

#define SW_USE_FOAM                                  // включить "пену" прибоя
#define SW_FOAM_THICKNESS half (0.035)               // толщина "пены"
#define SW_WATER_INTENSITY half (1.0)                // глубина цвета воды

// Shadow
#define ECB_SHADOW_KERNEL 2.4f                      // ширина одиночных полосок-переходов в тенях
#define ECB_SHADOW_STEPS 2                          // количество полосок перехода тени от светлой к темной (integer)

#endif
