#ifndef ME_DEBUG_H
#define ME_DEBUG_H

#define ME_DEBUG_LOG_COLOR(color) printf("r: %d, g: %d, b: %d\n", color.r, color.g, color.b)
#define ME_DEBUG_LOG_COORDINATES(position) printf("x : %f, y : %f\n", position.x, position.y) 
#define ME_DEBG_LOG_RECT(rect) printf("x : %d, y : %d, w : %d, h : %d\n", rect.x, rect.y, rect .width, rect.height)

#define ME_ASSERT(statement) if(!(statement)) printf("Asserton %s Failed @ line : %d in file : %s\n", #statement, __LINE__, __FILE__);  

#endif //ME_DEBUG_H