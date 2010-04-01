#!/usr/bin/env js

//

var Caos = {
  Statements : {},
  Expressions : {}
}

Caos.Statements.newsimp = function() {
  var family = pop_int
  var species = pop_int
  var genus = pop_int
  var sprite_file = pop_string
  var image_count = pop_int
  var first_image = pop_int
  var plane = pop_int

  print ("Creating new simple agent")
}

//

var Type = {
  Func : 'CaosFunction',
  Num : 'CaosNumber',
  Str: 'CaosString'
}

function func (word) {
  return [Type.Func, word]
}

function num (i) {
  return [Type.Num, i]
}

function str (s) {
  return [Type.Str, s]
}

var script = [
  func ('new: simp'),
  num (2),
  num (2),
  num (4),
  num (50000),
  str ("basicplant"),
  num (3),
  num (0),
  func ('rand'),
  num (200),
  num (6000)
]

var ip = 0
var stack = []

function peek() {
  return script[ip]
}

function shift() {
  return script[ip++]
}


while (ip != script.length) {
  pop_command()
}
