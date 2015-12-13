# -*- coding: utf-8 -*-


%w[xot beeps rays reflex]
  .map  {|s| File.expand_path "../../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflexion/include'


$player = $ground = nil
$left = $right = false

class View
  def remove_self ()
    parent.remove self if parent
  end
  alias center_noangle center
  def center
    p, c, a = pos, center_noangle, angle
    a == 0 ? c : p + (c - p).rotate(a)
  end
end

class KeyEvent
  def left? ()
    code == 123
  end
  def right? ()
    code == 124
  end
  def space? ()
    code == 49
  end
end

setup do
  size 640, 320
  flow :none
  gravity 0, 9.8 * meter

  root.wall.clear_fixtures
  add_ground
  add_bricks
  add_player
end

def add_ground ()
  $ground = window.add View.new {
    width 10000
    height parent.height
    static true
  }

  w, h = $ground.width, $ground.height
  $ground.body.clear_fixtures
  $ground.body.add_edge *[0, 0, 0, h, w, h, w, 0].each_slice(2)
end

def add_bricks ()
  range = $ground.frame.inset_by(100).tap {|f| break f.left .. f.right}
  50.times do
    add_brick rand(range), 100
  end
end

def add_brick (x, y)
  window.add RectShape.new {
    pos x, y
    size *(0..1).map {rand(20..100)}
    fill rand, rand, rand
    dynamic true
    density 1
  }
end

def add_player ()
  $player = window.add EllipseShape.new {
    @jump_count = 0

    pos 50, 50
    size 30
    dynamic true
    density 1
    friction 1

    player_image = Image.new(*size.to_a).paint do |p|
      step, colors = 5, [:green, :blue]
      (0..height).step step do |y|
        (0..width).step step do |x|
          p.fill colors[(x + y) % 2]
          p.rect x, y, step, step
        end
      end
    end

    def self.jumpable? ()
      @jump_count <= 1
    end
    def self.jump ()
      return unless jumpable?
      velocity velocity.tap {|v| v.y = -5 * meter}
      @jump_count += 1
    end

    on :update do
      dir = 0
      dir -= 1 if $left
      dir += 1 if $right
      self.angular_velocity = 360 * 3 * dir
    end
    on :draw do |e|
      e.painter.push fill: :white do
        image player_image
      end
    end
    on :contact_begin do
      @jump_count = 0
    end
  }
end

update do
  old_x, new_x = window.root.scroll.x, $player.center.x - window.width / 2
  window.root.scroll_to (old_x + new_x) / 2, 0
end

key do
  next unless down? || up?
  $left  = down? if left?
  $right = down? if right?
  $player.jump   if space? && down?
end
