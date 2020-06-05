
// lecture_3 반복문 연습

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_3
{
    class Program
    {
        static void Main(string[] args)
        {
            // for문
            for(int i = 0; i < 10; i++)
            {
                Console.WriteLine("Loop {0}", i);
            }

            // foreach 문 (for문보다 효율적으로 되어있음?)
            string[] str = new string[] {"I ", "My ", "Me "};
            
            foreach (string s in str)
            {
                Console.Write(s);
            }

            List<char> CharList = new List<char>();
            CharList.Add('a');
            CharList.Add('b');
            CharList.Add('c');

            foreach(char ch in CharList)
            {
                Console.Write(ch);
            }

            // while 문
            while (true)
            {
                Console.Write("\n");

                int i = 0;

                i++;

                if (i == 1)
                    break;
            }

            // do ~ while
            do
            {
                Console.WriteLine("End");

            } while (false);

            Console.ReadKey();
        }
    }
}
