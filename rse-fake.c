#include <linux/string.h>
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

/*
 * Some bookkeeping
 */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fake RSE Module for testing UI");


/*******************************************************************************
 * Begin actual module code
 ******************************************************************************/

/* 
 * Start with some function prototypes
 */
int create_procfile(struct proc_dir_entry **proc_file, const char * name, 
                    umode_t mode, struct proc_dir_entry *parent);
int read_proc_numprocs(char *page, char **start, off_t offset, int count, 
                        int *eof, void *data);
int write_proc_numprocs(struct file *file, const char *buf, 
                         unsigned long  count, void *data);

//PROC_INT_BUF_SIZE is the size (in bytes) of buffers used to read/write ints
//from proc files
#define PROC_INT_BUF_SIZE 10

//number of processes
int numprocs;

/* /proc files to access statistics and configuration */
struct proc_files {
  struct proc_dir_entry *numprocs;
  struct proc_dir_entry *parent;
} proc_files;

int __init init_module(void) {
  int result;
  
  numprocs = 0;
  /* Now create /proc files */
  proc_files.parent = proc_mkdir("rse", NULL);
  create_procfile(&(proc_files.numprocs), "numprocs", 0644,
                  proc_files.parent);
  proc_files.numprocs->read_proc = read_proc_numprocs;
  proc_files.numprocs->write_proc = write_proc_numprocs;

  /* All good if we made it this far */
  printk(KERN_INFO "rse_fake: loaded\n");
  return 0;
}

void __exit cleanup_module(void) {
  remove_proc_entry("numprocs", proc_files.parent);
  remove_proc_entry("rse", NULL); //Dir goes last

  printk(KERN_INFO "rse_fake: unloaded");
}


int create_procfile(struct proc_dir_entry **proc_file, const char * name, 
                    umode_t mode, struct proc_dir_entry *parent) {

  *proc_file = create_proc_entry(name,  mode, parent);

  if(proc_file == NULL) {
    remove_proc_entry(name, NULL);
    printk(KERN_ALERT "Error: Could not create /proc/%s\n", name);
    return -ENOMEM;
  }
  return 0;
}

int read_proc_numprocs(char *buf, char **start, off_t offset, int count, 
                        int *eof, void *data) {
  int len=1;
  len += sprintf(buf, "%d\n",numprocs);
    
  *eof = 1; 
  return len;
}

int write_proc_numprocs(struct file *file, const char *buf, 
                         unsigned long count, void *data) {
  char proc_data[PROC_INT_BUF_SIZE+1];
  int temp;

  if(count>PROC_INT_BUF_SIZE);
    count = PROC_INT_BUF_SIZE-1;

  if(copy_from_user(proc_data, buf, count))
    return -EFAULT;

  //Ensure that we end in a NULL character so that atoi doesn't go crazy
  proc_data[count+1] = '\0';
  temp = (int) simple_strtoul(proc_data, NULL, 10);
  printk(KERN_INFO "rse_fake: read value %d into numprocs\n", temp);
    
  numprocs = temp;

  return count;
}
