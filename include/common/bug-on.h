#ifndef __LABWC_BUG_ON_H
#define __LABWC_BUG_ON_H

#define BUG_ON(condition)                                             \
	do {                                                          \
		if ((condition) != 0) {                               \
			fprintf(stderr, "Badness in %s() at %s:%d\n", \
				__func__, __FILE__, __LINE__);        \
		}                                                     \
	} while (0)

#endif /* __LABWC_BUT_ON_H */
