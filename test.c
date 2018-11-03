

//条件语句不能加数组访问
//条件语句不能直接单值
//main 需要 return 0
//仅支持一维数组

// int main()
// {
//   int a =0,b=1,i=0,n;
//   n = read();
//   for(;i<n; i = i+1)
//   {
//     int c = a+b;
//     write(b);
//     a = b;
//     b= c;
//   }
//   return 0;
// }

// float test;
// int fact(int n)
// {
//   if(n == 1)
//    return n;
//   else 
//     return (n*fact(n-1));
// }
// int main()
// {
//   int m,result;
//   m = read();
//   if(m>1)
//     result = fact(m);
//   else 
//     result = 1;
//   write(result);
//   return 0;
// }


int a,b,c;
float m,n[3];
int fibo(int a) 
{
  if (a==1|| a==2) return 1;
  return fibo(a-1) + fibo(a-2);
}
int main() //main function define
{
  int m,n,i;
  m = read();
  i = 1;
  while(i<=m)
  {
    n = fibo(i);
    write(n);
    i = i+1;
  }
  return 1;
}

// int main()
// {
//   int arr[3] = {3,2,1},i,max=-1;
//   for(i=0;i<3;i++)
//   {
//     if(max<arr[i])
//       max = arr[i];
//   }
//   write(max);
//   return 0 ;
// }