
// lecture_4 Yield

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_4
{
    class Program
    {
        // yield의 사용, 처음에는 10, 두번째에는 20, 세번째에는 30 return
        static IEnumerable<int> GetNumber()
        {
            yield return 10;
            yield return 20;
            yield return 30;
        }

        static void Main(string[] args)
        {
            foreach (int i in GetNumber())
                Console.WriteLine(i);

            Console.ReadKey();
        }
    }
}
