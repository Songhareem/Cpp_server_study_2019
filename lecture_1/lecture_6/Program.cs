
// lecture_6 Class / Partial

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lecture_6
{
    // main 부
    partial class Program
    {
        static void Main(string[] args)
        {
        }
    }

    // 정의 및 선언부
    partial class Program
    {
        // 필드
        private string name;
        private int age;

        // 이벤트
        public event EventHandler NameChanged;

        // 생성자
        public Program()
        {
            name = string.Empty; // 빈문자열
            age = -1;
        }

        // 속성
        public string Name
        {
            get { return this.name; }
            set
            {
                if (this.name != value)
                {
                    this.name = value;
                    if (NameChanged != null)
                    {
                        NameChanged(this, EventArgs.Empty);
                    }
                }
            }
        }

        public int Age
        {
            get { return this.age; }
            set { this.age = value; }
        }

        // 메소드
        public string GetProgramData()
        {
            string data = string.Format("Name: {0} (Age:{1})", this.name, this.age);

            return data;
        }
    }
}
