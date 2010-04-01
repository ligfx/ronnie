#!/usr/bin/env ruby

require 'pp'

<<END
struct Value {
  enum {
    INT,
    STRING,
    BOOL,
    AGENT
  } type;
  union {
    int i;
    char *s;
    bool b;
    AgentRef a;
  } value;
};

struct Token {
  enum {
    VALUE,
    FUNCTION
  } type;
  union {
    Value v;
    char *f;
  }
};
END


# reps 2
# new: simp 2 4 50000 "basicplant" 3 0 rand 200 6000
# repe
#
# doif 1
# dbg: outs "Doif"
# endi
#
# doif 0
# dbg: outs "Error!"
# elif 0
# dbg: outs "Elif"
# else
# dbg: outs "Error!"
# endi
#
# doif 0
# dbg: outs "Error!"
# else
# dbg: outs "Else"
# endi

@secondary_functions = [
  "new:",
  "dbg:"
]

@statements = {
  "new: simp" => :caos_newsimp,
  "reps" => :caos_reps,
  "repe" => :caos_repe,
  "doif" => :caos_doif,
  "dbg: outs" => :caos_dbg_outs,
  "dbg: outi" => :caos_dbg_outi,
  "endi" => :caos_endi,
  "elif" => :caos_elif,
  "else" => :caos_else
}

@expressions = {
  "rand" => :caos_rand
}

@script = [
  [:symbol, "reps"],
  [:int, 2],
    [:symbol, "new:"],
    [:symbol, "simp"],
    [:int, 2],
    [:int, 4],
    [:int, 50000],
    [:string, "basicplant"],
    [:int, 3],
    [:int, 0],
      [:symbol, "rand"],
      [:int, 200],
      [:int, 6000],
  [:symbol, "repe"],
  [:symbol, "doif"],
  [:int, 1], [:condition, :caos_eq], [:int, 1], [:condition, :caos_and], [:int, 0], [:condition, :caos_eq], [:int, 0],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Doif"],
  [:symbol, "endi"],
  [:symbol, "doif"],
  [:int, 0], [:condition, :caos_eq], [:int, 7],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Error!"],
  [:symbol, "elif"],
  [:int, 1], [:condition, :caos_eq], [:int, 1],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Elif"],
  [:symbol, "else"],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Error!"],
  [:symbol, "endi"],
  [:symbol, "doif"],
  [:int, 0], [:condition, :caos_eq], [:int, 7],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Error!"],
  [:symbol, "else"],
    [:symbol, "dbg:"],
    [:symbol, "outs"],
    [:string, "Else"],
  [:symbol, "endi"],
  [:symbol, "dbg:"],
  [:symbol, "outi"],
  [:var, :va00]
]

@ip = 0
@stack = []

#

def peek
  return @script[@ip]
end

def shift
  tok = @script[@ip]
  @ip += 1
  return tok
end

def var_get (name)
  return 42
end

def pop_val (tok, type)
  if tok[0] == type
    return tok[1]
  elsif tok[0] == :symbol
    return send @expressions[tok[1]]
  elsif tok[0] == :var
    return var_get(tok[1])
  else
    puts "[ERROR] Expected #{type}, got #{tok[0]} @ #@ip" 
    abort
  end
end

def pop_int
  pop_val shift, :int
end

def pop_string
  pop_val shift, :string
end

def pop_expr # Int, float, string, expr
  tok = shift
  case tok[0]
  when :int
    return tok[1]
  when :float
    return tok[1]
  when :string
    return tok[1]
  when :expr
    return send tok[2]
  else
    puts "[ERROR] Expected int, float, string, or r-value, got #{tok[0]} @ #@ip"
    abort
  end 
end

def pop_condition
  # Shunting yard algorithm
  stk = []
  ops = []
  stk.push pop_expr
  begin
    tok = pop_val(shift, :condition)
    while not ops.empty? and tok == :caos_and
      right, left = stk.pop, stk.pop
      stk.push send(ops.pop, left, right)
    end
    ops.push tok

    stk.push pop_expr
  end while peek[0] == :condition

  until ops.empty?
    left, right = stk.pop, stk.pop
    stk.push send(ops.pop, left, right)
  end

  return stk.last
end

def pop_command
  tok = shift
  if tok[0] != :symbol
    puts "Expected symbol, got #{tok} @ #@ip"
    abort
  end
  sym = tok[1]
  msg = @statements[sym]
  if !msg
    if @secondary_functions.include? sym
      tok = shift
      if tok[0] != :symbol
        puts "Expected symbol, got #{tok} @ #@ip"
        abort
      end
      msg = @statements["#{sym} #{tok[1]}"]
    end
  end

  if msg
    send msg
  else
    puts "Couldn't find command!"
    abort
  end
end

#

def caos_eq left, right
  return left == right
end

def caos_and left, right
  return (left and right)
end

def caos_reps
  # Stack [...]
  times = pop_int
  @stack.push @ip
  @stack.push times
  # Stack [..., ip, times]
end

def caos_repe
  # Stack [..., ip, times]
  times = @stack.pop - 1
  if times == 0 # Done!
    @stack.pop # ip
    # Stack [...]
  else
    @ip = @stack.last
    @stack.push times
    # Stack [..., ip, times]
  end
end

def caos_doif
  # Stack [...]
  bool = pop_condition
  if bool == true # True
    @stack.push true # True
  else
    @stack.push false
    @ip += 1 until @script[@ip][0] == :symbol \
             and @script[@ip][1].to_s =~ /(elif)|(else)|(endi)/
  end
  # Stack [..., matched]
end

def caos_elif
  # Stack [..., matched]
  bool = pop_condition
  if @stack.last == true or bool == false # Matched or Condition:False
    @ip += 1 until @script[@ip][0] == :symbol \
             and @script[@ip][1].to_s =~ /(elif)|(else)|(endi)/
  else # Condition:True
    @stack.pop # Matched
    @stack.push true # True
  end
end

def caos_else
  # Stack [..., matched]
  if @stack.last == true # Matched
    @ip += 1 until @script[@ip] == [:symbol, "endi"]
  end
end

def caos_endi
  #Stack [..., matched]
  @stack.pop
  #Stack [...]
end

def caos_dbg_outs
  puts "[DEBUG] #{pop_string}"
end

def caos_dbg_outi
  puts "[DEBUG] #{pop_int}"
end

def caos_rand
  left = pop_int
  right = pop_int
  return (rand * (right - left)).round + left
end

def caos_newsimp
  family = pop_int
  genus = pop_int
  species = pop_int
  sprite_file = pop_string
  image_count = pop_int
  first_image = pop_int
  plane = pop_int

  puts "Creating simple agent"
  puts "Family #{family}"
  puts "Genus #{genus}"
  puts "Species #{species}"
  puts "Sprite File \"#{sprite_file}\""
  puts "Image Count #{image_count}"
  puts "First Image #{first_image}"
  puts "Plane #{plane}"
  puts
end

#

def main()
  pop_command() until (@ip == @script.length)
end

main()
