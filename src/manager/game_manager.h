
#include "manager.h"

class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;

public:
protected:
    GameManager()  = default;
    ~GameManager() = default;
};
