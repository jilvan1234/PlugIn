.DATA        ;定义数据段

IBinary DQ ?  ;自定义了一个常量为IBinary

.CODE         ;定义代码段

 

_GetKenelBase PROC

   mov rcx,gs:[0x2b];
   
   ret

_GetKenelBase endp
 


END