class d_node {
public:
	/* Attributes */
	int label;
	char byte;
	d_node* parent;
	d_node* left;
	d_node* right;
	bool initialized;
	
	/* Methods */
	d_node();
	d_node(char c, int l, d_node* p);
	d_node* find_entry(char c);
	d_node* insert_entry(char byte, int label);
};