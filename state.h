#ifndef STATE_H
#define STATE_H

class State
{
public:
    static State& GetInstance()
    {
        static State state;
        return state;
    }

    bool paused_ = false;

private:
    State()
    {
    }

};

#endif // STATE_H
