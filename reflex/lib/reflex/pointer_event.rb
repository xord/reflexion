# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class PointerEvent

    alias pos position

    def type ()
      TYPE2SYM[get_type] || :none
    end

    def down? ()
      get_type == TYPE_DOWN
    end

    def up? ()
      get_type == TYPE_UP
    end

    def move? ()
      get_type == TYPE_MOVE
    end

    def left? ()
      pointer_type.include? :mouse_left
    end

    def right? ()
      pointer_type.include? :mouse_right
    end

    def middle? ()
      pointer_type.include? :mouse_middle
    end

    def touch? ()
      pointer_type.include? :touch
    end

    def pen? ()
      pointer_type.include? :pen
    end

    def pointer_type ()
      type_, array = get_pointer_type, []
      POINTER_TYPE_LAST.to_s(2).sub(/^0+/, '').size.times do |i|
        bit = 0x1 << i
        if (type_ & bit) != 0
          sym = POINTERTYPE2SYM[bit]
          array << sym if sym
        end
      end
      array
    end

    def inspect ()
      "#<Reflex::PointerEvent type:#{type}/#{pointer_type} x:#{x} y:#{y} size:#{size} mod:#{modifiers} count:#{count} drag:#{drag?}>"
    end

    private

      TYPE2SYM = {
        TYPE_DOWN => :down,
        TYPE_UP   => :up,
        TYPE_MOVE => :move
      }

      POINTERTYPE2SYM = {
        POINTER_MOUSE_LEFT   => :mouse_left,
        POINTER_MOUSE_RIGHT  => :mouse_right,
        POINTER_MOUSE_MIDDLE => :mouse_middle,
        POINTER_TOUCH        => :touch,
        POINTER_PEN          => :pen,
      }

  end# PointerEvent


end# Reflex
