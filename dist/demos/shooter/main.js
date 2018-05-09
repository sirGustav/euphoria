print('Hello world');

IsDown = function(key) {
  return key.state > 0.1
}
WasDown = function(key) {
  return key.last_state > 0.1
}
JustPressed = function(key) {
  return IsDown(key) && !WasDown(key);
}

Types = {
  Pos2: Registry.GetPosition2Id(),
  Player: Registry.New("Player"),
  MoveUp: Registry.New("MoveUp"),
  TimeOut: Registry.New("TimeOut", function(c) {
    c.time = 1;
  })
};

time = 0;
bark = 0;
Systems.AddUpdate("bark", function(dt) {
  time += dt;
  if(time>1){
    time -= 1;
    bark += 1;
    print('Bark!', bark);
  }
});

Systems.AddUpdate("move up", function(dt) {
  var ents = Registry.Entities([Types.Pos2, Types.MoveUp]);
  ents.forEach(function(entity) {
    var vec = Registry.GetPosition2vec(entity);
    if(vec != null)
    {
      var speed = 250;

      vec.y = vec.y + dt * speed;
    }
  });
});

Systems.AddUpdate("time out", function(dt) {
  var ents = Registry.Entities([Types.TimeOut]);
  ents.forEach(function(entity) {
    var data = Registry.Get(entity, Types.TimeOut);
    data.time -= dt;
    if(data.time < 0)
    {
      print("timeouted");
      data.time += 10;
    }
  });
});

shotTemplate = Templates.Find("shot");
Systems.AddUpdate("player", function(dt) {
  ents = Registry.Entities([Types.Pos2, Types.Player]);
  ents.forEach(function(entity) {
    var vec = Registry.GetPosition2vec(entity);
    if(vec != null)
    {
      var speed = 150;

      var vertical = Input.up.state - Input.down.state;
      var horizontal = Input.right.state - Input.left.state;

      if(JustPressed(Input.fire))
      {
        if(!shotTemplate)
        {
          print("no shot");
        }
        else
        {
          var shot = shotTemplate.Create();
          var v = Registry.GetPosition2vec(shot);
          if(v != null)
          {
            v.x = vec.x;
            v.y = vec.y;
          }
        }
      }
      vec.y = vec.y + dt * speed * vertical;
      vec.x = vec.x + dt * speed * horizontal;
    }
  });
});
