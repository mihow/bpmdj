// memory
#define allocate(size, type) (type*)myalloc(sizeof(type)*(size), __FILE__, __LINE__)
#define array(name,size,type) type * name = allocate(size,type)
#define reallocate(thing, size, type) (type*)myrealloc(thing,sizeof(type)*(size))
#define deallocate(thing) myfree(thing);

