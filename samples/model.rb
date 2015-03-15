# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'

include Reflex


class SliderView < View

  has_model

  def initialize ()
    add @back = RectShape.new {set name: :back, border: :white, follow: :all}
    add @knob = RectShape.new {set name: :knob, border: :white, follow: [:top, :bottom], color: :gray, width: 50, x: 50}

    start = nil
    @knob.on :pointer do |e|
      case e.type
      when :down
        start = e.position
        @knob.capture :pointer
      when :up
        start = nil
        @knob.capture :none
      when :move
        self.data = (@knob.x + e.x - start.x) / knob_left_max.to_f if start
      end
    end

    super
  end

  def on_data_update (e)
    @knob.x = knob_left_max * data.to_f
    super
  end

  private

    def knob_left_max ()
      width - @knob.width
    end

end# SliderView


class LabelView < View

  has_model

  def on_draw (e)
    e.painter.text data.to_s if data
  end

end# LabelView


class ArrayView < View

  has_model

  def on_draw (e)
    e.painter.text "[#{data.to_s.chars.map{|c| "'#{c}'"}.to_a.join(', ')}]" if data
  end

end# ArrayView


win = Window.new do
  add @slider = SliderView.new {set name: :slider, frame: [50, 50, 300, 30]}
  add @text   = LabelView.new  {set name: :text,   frame: [50, 150, 300, 30]}
  add @array  = ArrayView.new  {set name: :array,  frame: [50, 250, 300, 30]}

  @slider.model = @text.model = @array.model = Model.new(0) {set minmax: 0..1}

  set title: 'Model View Sample', frame: [100, 100, 500, 400]
end


Reflex.start do
  win.show
end
