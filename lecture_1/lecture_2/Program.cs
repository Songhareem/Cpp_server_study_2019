
// lecture_2 class 전역변수, 지역변수, Readonly, const

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_2
{
    class CsVal
    {
        // 클래스 전역변수
        int iVal;
        const int iAngel = 1004;

        // readonly 변수는 Const와 비슷, 근데 런타임에 할당 받음, 
        // 클래스 전역변수에서만 선언 가능, 생성자에서만 초기화 가능
        readonly int iReadVal;

        public void print()
        {
            // 지역변수에 값 넣어주지 않으면 에러
            int Error = 0;

            string szDevil = "Devil";

            Console.WriteLine(szDevil);
            Console.WriteLine(iAngel);
            Console.WriteLine(iReadVal);
        }

        public CsVal(int iVal)
        {
            iReadVal = iVal;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            CsVal Val = new CsVal(10);

            Val.print();

            Console.ReadKey();
        }
    }
}
