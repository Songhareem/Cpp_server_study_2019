
// lecture_1 WriteLine, ReadKey, 변수형 정리

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_1
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("HelloWorld");

            // scanf
            Console.ReadKey();

            bool b = true;
            byte by = 0xff;
            int i = 1;
            long lg = 2L;
            float f = 3.0F;
            double d = 4.0D;
            decimal de = 10M;

            char c = 'c';
            string str = "string";

            // NULL 타입 가지는 변수 Reference Type / NULL 타입을 가지지 못하는 변수 Value Type
            string nullStr = null;

            // NULLable Type -> Value Type에 NULL 값을 넣고 싶을 때 사용
            int? _i = null;

            bool? _b = null;
        }
    }
}