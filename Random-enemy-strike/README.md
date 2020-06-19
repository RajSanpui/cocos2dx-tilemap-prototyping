In case of random enemy strike, any sprite can throw the projectile.

    Sprite *enemy[4] = {_enemy4, _enemy3, _enemy2, _enemy1};
    Sprite *closestEnemy = enemy[random(0,3)];


    auto projectile = Sprite::create("bomba.png");
    projectile->setPosition(closestEnemy->getPosition());
    this->addChild(projectile);
