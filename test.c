//条件语句不能加数组访问
// int main ()
// {
//   int i = 5;
//   for(;i>0;i--)
//   {
//     write(i);
//   }
//   return 0;
// }

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


// int fibo(int a) 
// {
//   if (a==1|| a==2) return 1;
//   return fibo(a-1) + fibo(a-2);
// }
// int main() 
// {
//   int m,n,i;
//   int j = 1; 
//   m = read();
//   i = 1;
//   while(i<=m)
//   {
//     n = fibo(i);
//     write(n);
//     i = i+1;
//   }
//   return 1;
// }
