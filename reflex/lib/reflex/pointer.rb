# -*- coding: utf-8 -*-


require 'xot/bit_flag_accessor'
require 'xot/const_symbol_accessor'
require 'reflex/ext'


module Reflex


  class Pointer

    include Comparable

    alias type   get_type
    alias action get_action

    bit_flag_reader :type, **{
      none:         TYPE_NONE,
      mouse_left:   MOUSE_LEFT,
      mouse_right:  MOUSE_RIGHT,
      mouse_middle: MOUSE_MIDDLE,
      touch:        TOUCH,
      pen:          PEN
    }

    const_symbol_reader :action, **{
      none:   ACTION_NONE,
      down:   DOWN,
      up:     UP,
      move:   MOVE,
      cancel: CANCEL,
      stay:   STAY
    }

    def mouse?()
      (get_type & (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)) != 0
    end

    def mouse_left?()
      (get_type & MOUSE_LEFT) != 0
    end

    def mouse_right?()
      (get_type & MOUSE_RIGHT) != 0
    end

    def mouse_middle?()
      (get_type & MOUSE_MIDDLE) != 0
    end

    alias left?   mouse_left?
    alias right?  mouse_right?
    alias middle? mouse_middle?

    def touch?()
      (get_type & TOUCH) != 0
    end

    def pen?()
      (get_type & PEN) != 0
    end

    def down?()
      get_action == DOWN
    end

    def up?()
      get_action == UP
    end

    def move?()
      get_action == MOVE
    end

    def cancel?()
      get_action == CANCEL
    end

    def stay?()
      get_action == STAY
    end

    alias pos position

    def x()
      position.x
    end

    def y()
      position.y
    end

    def <=>(o)
      inspect <=> o.inspect
    end

    def inspect()
      "#<Reflex::Pointer type:#{type} act:#{action} x:#{x} y:#{y} mod:#{modifiers} click:#{click_count} drag:#{drag?}>"
    end

  end# Pointer


end# Reflex
