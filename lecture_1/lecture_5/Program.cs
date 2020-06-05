
// lecture_5 Struct

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_5
{
    class Program
    {
        // Struct define
        struct MyPoint
        {
            public int X;
            public int Y;

            public MyPoint(int iX, int iY)
            {
                this.X = iX;
                this.Y = iY;
            }

            public override string ToString()
            {
                return string.Format("({0}, {1})", X, Y);
            }
        }

        static void Main(string[] args)
        {
            // Struct 사용
            MyPoint pt = new MyPoint(10, 12);

            Console.WriteLine(pt.ToString());

            Console.Read();
        }
    }
}
