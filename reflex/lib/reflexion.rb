# -*- coding: utf-8 -*-


require 'rays'
require 'reflex'


module Reflexion


  include Rays, Reflex


  DEFAULTS = {
    title: 'Reflexion',
    frame: [100, 100, 512, 512]
  }


  class MainWindow < Window

    attr_accessor :update, :draw, :key, :pointer

    attr_reader :setup

    attr_reader :event

    def setup= (block)
      @setup = block
      call_event nil, self, &@setup
    end

    def on_update (e)
      super
      redraw
      call_event e, e, &@update
    end

    def on_draw (e)
      super
      call_event e, e.painter, &@draw
    end

    def on_key (e)
      super
      call_event e, e, &@key
    end

    def on_pointer (e)
      super
      call_event e, e, &@pointer
    end

    def call_event (event, *args, &block)
      @event = event
      Xot::BlockUtil.instance_eval_or_block_call *args, &block if block
    end

  end# MainWindow


  module_function

  @@window = MainWindow.new DEFAULTS

  def window ()
    @@window
  end

  def event ()
    window.event
  end

  def setup (&block)
    window.setup = block
  end

  def update (&block)
    window.update = block
  end

  def draw (&block)
    window.draw = block
  end

  def key (&block)
    window.key = block
  end

  def pointer (&block)
    window.pointer = block
  end

  def start ()
    window.show
    Reflex.start
  end

  def quit ()
    window.close
  end


end# Reflexion


at_exit do
  Reflexion.start unless $!
end
