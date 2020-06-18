If the soldier is between multiple enemy sprites, the enemy which is closest to the soldier makes the strike.

    Sprite *closestEnemy;
    
    auto closest = abs(-_player->getPosition().x + _enemy4->getPosition().x);
    auto temp = abs(-_player->getPosition().x + _enemy3->getPosition().x);

    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy3;
    }
    else {
        closestEnemy = _enemy4;
    }

    temp = abs(- _player->getPosition().x + _enemy2->getPosition().x);
    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy2;
    }

    temp = abs(- _player->getPosition().x + _enemy1->getPosition().x);
    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy1;
    }
