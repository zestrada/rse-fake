rse-fake
========

Fake kernel module to simulate RSE for UI development

Right now, it acts as an extremely simple proc file:
```
root@test:~/rse-fake# cat /proc/rse/numprocs 
0
root@test:~/rse-fake# echo "1" > /proc/rse/numprocs 
root@test:~/rse-fake# cat /proc/rse/numprocs 
1
```
