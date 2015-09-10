# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


class V < Reflex::View

  def initialize (offset = [0, 0])
    super()
    frame *offset, 400, 200
  end

  def on_update (e)
    scroll_to *$scroll if $scroll
  end

  def on_draw (e)
    e.painter.push do
      b = e.bounds.to_a

      fill 0.2
      stroke 0.1
      rect *b

      fill :green
      stroke :red
      text e.fps.to_i.to_s + " FPS."

      fill :none
      stroke 1, 0, 0, 0.4
      rect *b
    end
  end

end# V


class W < Reflex::Window

  def initialize ()
    set :title, "View Tree Sample"
    set :frame, 100, 100, 320, 240

    painter.background 0
    painter.fill 1
    painter.font nil, 20
  end

  def on_pointer_move (e)
    $scroll = [-e.x.floor, -e.y.floor]
  end

end# W


def setup (parent, level)
  return unless level > 0
  1.times do |i|
    v = V.new [i * 10] * 2
    parent.add v
    setup v, level - 1
  end
end


Reflex.start do
  w = W.new
  setup w, 5
  w.show
end
