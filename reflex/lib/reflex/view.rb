# -*- coding: utf-8 -*-


require 'forwardable'
require 'xot/setter'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/model_view'
require 'reflex/flags'
require 'reflex/helper'


module Reflex


  class View

    include Xot::Setter
    include Hookable
    include HasFrame
    include HasTags

    extend Forwardable

    def_delegators :body,
      :static=, :static?, :dynamic=, :dynamic?,
      :velocity=, :linear_velocity=, :angular_velocity=,
      :velocity,  :linear_velocity,  :angular_velocity,
      :density=, :density, :friction=, :friction, :restitution=, :restitution

    alias add    add_child
    alias remove remove_child
    alias find   find_children
    alias meter meter2pixel

    def initialize (opts = {}, &block)
      super()
      set opts
      @attach_block = block if block
    end

    def children ()
      to_enum :each_child
    end

    def styles ()
      to_enum :each_style
    end

    def style (*args, &block)
      s = get_style args.empty? ? nil : Selector.selector(*args)
      Xot::BlockUtil.instance_eval_or_block_call s, &block if block
      s
    end

    def capture= (*args)
      set_capture CAPTURE_FLAG.symbols2bits(*args.flatten)
    end

    def capture (*args)
      send :capture=, *args unless args.empty?
      CAPTURE_FLAG.bits2symbols get_capture
    end

    def capturing? (*args)
      cap = capture
      args.all? {|type| cap.include? type}
    end

    def gravity= (*args)
      case arg = args[0]
      when Point then set_gravity arg.x, arg.y
      when Array then set_gravity arg[0], arg[1]
      else            set_gravity *args
      end
    end

    def self.has_model ()
      include ModelView
    end

    private

      def call_attach_block ()
        return unless @attach_block
        Xot::BlockUtil.instance_eval_or_block_call self, &@attach_block
        @attach_block = nil
      end

  end# View


end# Reflex
