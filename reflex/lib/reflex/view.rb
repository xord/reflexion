# -*- coding: utf-8 -*-


require 'forwardable'
require 'xot/setter'
require 'xot/bit_flag_accessor'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/model_view'
require 'reflex/helper'


module Reflex


  class View

    include Xot::Setter
    include Hookable
    include HasFrame
    include HasTags

    extend Forwardable

    def_delegators :style,
      :flow=,              :flow,
      :foreground_fill=,   :foreground_fill,
      :foreground_stroke=, :foreground_stroke,
      :background_fill=,   :background_fill,
      :background_stroke=, :background_stroke,
      :foreground=,        :foreground,
      :background=,        :background,
      :fore_fill=,         :fore_fill,
      :fore_stroke=,       :fore_stroke,
      :back_fill=,         :back_fill,
      :back_stroke=,       :back_stroke,
      :fill=,              :fill,
      :stroke=,            :stroke,
      :image=,             :image

    bit_flag_accessor :capture do
      flag :key,     CAPTURE_KEY
      flag :pointer, CAPTURE_POINTER
      flag :all,     CAPTURE_ALL
    end

    def initialize (options = nil, &block)
      super()
      set options if options
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def timeout (seconds = 0, count: 1, &block)
      timer = start_timer seconds, count
      timer.block = block if block
      timer
    end

    def interval (seconds = 0, &block)
      timeout seconds, count: -1, &block
    end

    def find_child (*args)
      find_children(*args).first
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

    def shapes ()
      to_enum :each_shape
    end

    def categories ()
      @categories ||= Xot::BitFlag.new(auto: true, all: 1)
    end

    def category= (*symbols)
      set_category_bits parent_categories.symbols2bits *symbols
    end

    def category ()
      parent_categories.bits2symbols get_category_bits
    end

    def collision= (*categories)
      set_collision_mask parent_categories.symbols2bits *categories
    end

    def collision ()
      parent_categories.bits2symbols get_collision_mask
    end

    def capturing? (*args)
      cap = capture
      args.all? {|type| cap.include? type}
    end

    alias add    add_child
    alias remove remove_child
    alias find   find_child

    alias clip?  clip
    alias cache? cache

    alias apply_impulse apply_linear_impulse
    alias static?       static
    alias dynamic?      dynamic
    alias sensor?       sensor
    alias velocity=     linear_velocity=
    alias velocity      linear_velocity

    alias meter  meter2pixel
    alias debug? debug

    universal_accessor :shape, :name, :selector, :frame, :zoom, :capture,
      :clip, :cache,
      :static, :dynamic, :density, :friction, :restitution,
      :sensor, :category, :collision,
      :linear_velocity, :angular_velocity, :gravity_scale,
      :gravity, :debug

    def self.has_model ()
      include ModelView
    end

    protected

      def parent_categories ()
        raise InvalidStateError unless parent
        parent.categories
      end

  end# View


end# Reflex
