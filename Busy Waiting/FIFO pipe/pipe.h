char *circ_buffer;
volatile int head;
volatile int tail;
int size_of_input;

void pipe_init(int size);
void pipe_close();
int pipe_read (char *c);
void pipe_write(char c);
