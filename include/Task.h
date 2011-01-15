#ifndef _TASK_H_
#define _TASK_H_

namespace CLPIM
{
    class Task
    {
    public:
        Task()
        {
        }

        virtual ~Task()
        {
        }

        virtual void Exec() = 0;
    };
}

#endif // _TASK_H_
