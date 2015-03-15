# -*- coding: utf-8 -*-


require 'reflex/ext'
require 'reflex/window'


module Reflex


  extend module ClassMethods

    def start (*args, &block)
      Application.new(*args, &block).start
    end

    def quit ()
      Application.instance.quit
    end

    def window (*args, &block)
      w = Window.new *args, &block
      w.show
      w
    end

    self

  end# ClassMethods


end# Reflex
