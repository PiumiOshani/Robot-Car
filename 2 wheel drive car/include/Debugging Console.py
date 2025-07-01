import pygame
import sys
import socket
import threading
import time
from pygame.locals import *
from datetime import datetime

# Initialize Pygame
pygame.init()

# Constants
WINDOW_WIDTH = 1300  # Increased width to accommodate layout
WINDOW_HEIGHT = 600
FPS = 60

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (220, 220, 220)
DARK_GRAY = (100, 100, 100)
PRIMARY_BLUE = (41, 128, 185)
SECONDARY_BLUE = (52, 152, 219)
ACCENT_GREEN = (46, 204, 113)
ERROR_RED = (231, 76, 60)

# Network settings
UDP_IP = "192.168.1.8"
UDP_PORT = 12345

class NetworkManager:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', UDP_PORT))
        self.messages = []
        self.max_messages = 5
        self.last_received = ""
    
    def send_message(self, label, value):
        try:
            message = f"{label}={value}".encode()
            self.sock.sendto(message, (UDP_IP, UDP_PORT))
            return True
        except Exception as e:
            self.add_message(f"Send error: {str(e)}", is_error=True)
            return False
    
    def add_message(self, message, is_error=False):
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.messages.append({
            "text": f"[{timestamp}] {message}",
            "is_error": is_error
        })
        if len(self.messages) > self.max_messages:
            self.messages.pop(0)

class TextInputBox:
    def __init__(self, x, y, label_text, function, default_value="0"):
        self.label_text = label_text
        self.rect = pygame.Rect(x + 120, y, 100, 32)
        self.button = pygame.Rect(x + 230, y, 80, 32)
        self.text = default_value
        self.selected = False
        self.function = function
        self.value = 0
        self.last_update = time.time()
        self.update_interval = 0.1  # Minimum time between updates
        
        # Visual properties
        self.border_radius = 5
        self.font = pygame.font.Font(None, 24)
        self.label = self.font.render(label_text, True, BLACK)
        self.update_text_surface()
    
    def update_text_surface(self):
        self.text_surface = self.font.render(self.text, True, BLACK)
        
    def handle_event(self, event):
        if event.type == MOUSEBUTTONDOWN and event.button == 1:
            self.selected = self.rect.collidepoint(event.pos)
            if self.button.collidepoint(event.pos):
                self.send_value()
                
        elif event.type == KEYDOWN and self.selected:
            if event.key == K_RETURN:
                self.send_value()
            elif event.key == K_BACKSPACE:
                self.text = self.text[:-1]
            else:
                # Only allow numbers and decimal point
                if event.unicode.replace(".", "").isnumeric():
                    self.text += event.unicode
            self.update_text_surface()
    
    def send_value(self):
        if time.time() - self.last_update >= self.update_interval:
            try:
                value = float(self.text)
                self.value = value
                self.function(self.label_text, self.text)
                self.last_update = time.time()
            except ValueError:
                pass
    
    def draw(self, screen):
        # Draw label
        screen.blit(self.label, (self.rect.x - 110, self.rect.y + 6))
        
        # Draw input box
        border_color = PRIMARY_BLUE if self.selected else DARK_GRAY
        pygame.draw.rect(screen, WHITE, self.rect, border_radius=self.border_radius)
        pygame.draw.rect(screen, border_color, self.rect, 2, border_radius=self.border_radius)
        screen.blit(self.text_surface, (self.rect.x + 5, self.rect.y + 6))
        
        # Draw button
        button_color = ACCENT_GREEN
        pygame.draw.rect(screen, button_color, self.button, border_radius=self.border_radius)
        button_text = self.font.render("Update", True, WHITE)
        button_text_rect = button_text.get_rect(center=self.button.center)
        screen.blit(button_text, button_text_rect)

class BarGraph:
    def __init__(self, x, y, width, height):
        self.rect = pygame.Rect(x, y, width, height)
        self.bar_width = 20
        self.spacing = 10
        self.max_height = height - 40  # Leave room for labels
        self.font = pygame.font.Font(None, 20)
    
    def draw(self, screen, data):
        # Draw background
        pygame.draw.rect(screen, GRAY, self.rect, border_radius=10)
        
        # Draw graph title
        title_font = pygame.font.Font(None, 24)
        title = title_font.render("Response Values", True, BLACK)
        screen.blit(title, (self.rect.x + 10, self.rect.y - 30))
        
        # Draw bars
        x = self.rect.x + 20
        for i, value in enumerate(data):
            # Calculate bar height (scale to percentage)
            height = (value / 100) * self.max_height
            bar_rect = pygame.Rect(x, self.rect.y + self.max_height - height, 
                                 self.bar_width, height)
            
            # Draw bar with gradient effect
            pygame.draw.rect(screen, SECONDARY_BLUE, bar_rect, border_radius=3)
            
            # Draw value label
            value_text = self.font.render(f"{value:.1f}", True, BLACK)
            text_rect = value_text.get_rect(centerx=x + self.bar_width/2,
                                          bottom=self.rect.bottom - 5)
            screen.blit(value_text, text_rect)
            
            x += self.bar_width + self.spacing

class Application:
    def __init__(self):
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption('Robot Control Panel')
        self.clock = pygame.time.Clock()
        
        self.network = NetworkManager()
        self.bar_data = [0] * 10
        
        # Create input boxes
        self.create_input_boxes()
        
        # Create bar graph
        graph_width = 400
        graph_height = 300
        self.bar_graph = BarGraph(
            780,  # Moved further right
            80,
            graph_width,
            graph_height
        )
        
        # Start UDP listener
        self.start_udp_listener()
    
    def create_input_boxes(self):
        labels = ["FWD_KP", "FWD_KD", "FWD_KI", 
                 "ROT_KP", "ROT_KD", "ROT_KI",
                 "STR_KP", "STR_KD", "STR_KI", "SPEED"]
        self.input_boxes = []
        
        for i, label in enumerate(labels):
            row = i % 5
            col = i // 5
            x = 20 + col * 320
            y = 80 + row * 50
            
            self.input_boxes.append(
                TextInputBox(x, y, label, self.network.send_message)
            )
    
    def start_udp_listener(self):
        def udp_listen():
            while True:
                try:
                    self.network.sock.settimeout(0.02)
                    data, addr = self.network.sock.recvfrom(1024)
                    message = data.decode()
                    
                    # Parse received data
                    try:
                        values = message.split(",")
                        for i, val in enumerate(values):
                            self.bar_data[i] = float(val)
                        self.network.last_received = message
                    except Exception as e:
                        self.network.add_message(f"Parse error: {str(e)}", True)
                
                except socket.timeout:
                    pass
                except Exception as e:
                    self.network.add_message(f"Receive error: {str(e)}", True)
                
                time.sleep(0.02)
        
        threading.Thread(target=udp_listen, daemon=True).start()
    
    def draw(self):
        self.screen.fill(WHITE)
        
        # Draw title
        title_font = pygame.font.Font(None, 36)
        title = title_font.render("Robot Control Panel", True, BLACK)
        self.screen.blit(title, (20, 20))
        
        # Draw input boxes section title
        section_font = pygame.font.Font(None, 24)
        params_title = section_font.render("Control Parameters", True, BLACK)
        self.screen.blit(params_title, (20, 50))
        
        # Draw input boxes
        for box in self.input_boxes:
            box.draw(self.screen)
        
        # Draw bar graph
        self.bar_graph.draw(self.screen, self.bar_data)
        
        # Draw messages
        message_y = WINDOW_HEIGHT - 150
        msg_title = section_font.render("System Messages", True, BLACK)
        self.screen.blit(msg_title, (20, message_y - 30))
        
        for msg in self.network.messages:
            color = ERROR_RED if msg["is_error"] else BLACK
            text = self.input_boxes[0].font.render(msg["text"], True, color)
            self.screen.blit(text, (20, message_y))
            message_y += 25
        
        pygame.display.flip()
    
    def run(self):
        while True:
            for event in pygame.event.get():
                if event.type == QUIT:
                    self.network.sock.close()
                    pygame.quit()
                    sys.exit()
                
                for box in self.input_boxes:
                    box.handle_event(event)
            
            self.draw()
            self.clock.tick(FPS)

if __name__ == "__main__":
    app = Application()
    app.run()
