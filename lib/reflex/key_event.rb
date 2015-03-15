# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class KeyEvent

    def type ()
      TYPE2SYM[get_type] || :none
    end

    def down? ()
      get_type == TYPE_DOWN
    end

    def up? ()
      get_type == TYPE_UP
    end

    def inspect ()
      "#<Reflex::KeyEvent type:#{type} chars:'#{chars}' code:#{code} mod:#{modifiers} repeat:#{repeat} capture?:#{capture?}>"
    end

    private

      TYPE2SYM = {
        TYPE_DOWN => :down,
        TYPE_UP   => :up,
      }

  end# KeyEvent


end# Reflex
