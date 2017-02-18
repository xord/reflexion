# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Style

    include Xot::Setter
    include HasTags

    def initialize (options = nil, &block)
      super()
      set options if options
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def flow= (*syms)
      set_flow *syms.flatten.map {|sym| sym2flow sym}
    end

    def flow ()
      get_flow.map {|n| flow2sym n}
    end

    def foreground= (fill, stroke = nil)
      self.foreground_fill   = fill
      self.foreground_stroke = stroke
    end

    def foreground ()
      return foreground_fill, foreground_stroke
    end

    def background= (fill, stroke = nil)
      self.background_fill   = fill
      self.background_stroke = stroke
    end

    def background ()
      return background_fill, background_stroke
    end

    alias x=   left=
    alias x    left
    alias y=   top=
    alias y    top
    alias w=   width=
    alias w    width
    alias h=   height=
    alias h    height
    alias pos= position=
    alias pos  position

    alias fill=   foreground_fill=
    alias fill    foreground_fill
    alias stroke= foreground_stroke=
    alias stroke  foreground_stroke

    universal_accessor :name, :selector, :flow, :width, :height, :size,
              :left,         :top,         :right,         :bottom, :position,
       :margin_left,  :margin_top,  :margin_right,  :margin_bottom, :margin,
      :padding_left, :padding_top, :padding_right, :padding_bottom, :padding,
      :center, :center_x, :center_y,
      :foreground, :foreground_fill, :foreground_stroke,
      :background, :background_fill, :background_stroke,
      :fill, :stroke, :image

    def inspect ()
      attrs = {
        selector:   selector,
        flow:       flow,
        size:       size,
        position:   position,
        margin:     margin,
        padding:    padding,
        center:     center,
        foreground: foreground,
        background: background,
        image:      image
      }
      "#<Reflex::Style #{attrs.map {|k, v| %(#{k}:#{v.to_s})}.join ' '}>"
    end

    private

      SYM2FLOW = {
        none:  FLOW_NONE,
        down:  FLOW_DOWN,
        right: FLOW_RIGHT,
        up:    FLOW_UP,
        left:  FLOW_LEFT,
      }

      FLOW2SYM = SYM2FLOW.inject({}) {|h, (k, v)| h[v] = k; h}

      def sym2flow (sym)
        SYM2FLOW[sym] || FLOW_NONE
      end

      def flow2sym (flow)
        FLOW2SYM[flow] || :none
      end

  end# Style


end# Reflex
