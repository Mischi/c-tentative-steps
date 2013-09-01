#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/cdefs.h>
#include <sys/conf.h>
#include <sys/exec.h>
#include <sys/lkm.h>
#include <sys/kernel.h>


MOD_MISC("helloworld")

int
helloworld_lkmload(struct lkm_table *lkmt, int cmd)
{
    uprintf("Hello World!\n");
    return 0;
}


int
helloworld_lkmentry(struct lkm_table *lkmt, int cmd, int ver) 
{
    DISPATCH(lkmt, cmd, ver, helloworld_lkmload, lkm_nofunc, lkm_nofunc);
}
