#ifndef LABEL_H
#define LABEL_H

struct Label{
	static unsigned counter; 
	unsigned number; 
	Label() { number = counter++; }
}; 

#endif /* LABEL_H */
