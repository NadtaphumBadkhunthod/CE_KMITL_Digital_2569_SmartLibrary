-- ============================================================
-- UART receiver
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity uart_rx is
  generic (CLKS_PER_BIT : integer := 434);   -- 50 MHz / 115200
  port (
    clk   : in  std_logic;
    rx    : in  std_logic;
    data  : out std_logic_vector(7 downto 0);
    valid : out std_logic
  );
end uart_rx;

architecture rtl of uart_rx is
  type state_t is (IDLE, START, DATA_BITS, STOP);
  signal state : state_t := IDLE;
  signal rx_s1, rx_s2 : std_logic := '1';
  signal cnt : integer range 0 to CLKS_PER_BIT - 1 := 0;
  signal idx : integer range 0 to 7 := 0;
  signal sh  : std_logic_vector(7 downto 0) := (others => '0');
begin
  process(clk)
  begin
    if rising_edge(clk) then
      rx_s1 <= rx;
      rx_s2 <= rx_s1;
      valid <= '0';

      case state is
        when IDLE =>
          cnt <= 0;
          idx <= 0;
          if rx_s2 = '0' then
            state <= START;
          end if;

        when START =>
          if cnt = CLKS_PER_BIT / 2 - 1 then
            cnt <= 0;
            if rx_s2 = '0' then
              state <= DATA_BITS;
            else
              state <= IDLE;
            end if;
          else
            cnt <= cnt + 1;
          end if;

        when DATA_BITS =>
          if cnt = CLKS_PER_BIT - 1 then
            cnt <= 0;
            sh(idx) <= rx_s2;
            if idx = 7 then
              state <= STOP;
            else
              idx <= idx + 1;
            end if;
          else
            cnt <= cnt + 1;
          end if;

        when STOP =>
          if cnt = CLKS_PER_BIT - 1 then
            cnt <= 0;
            if rx_s2 = '1' then
              data  <= sh;
              valid <= '1';
            end if;
            state <= IDLE;
          else
            cnt <= cnt + 1;
          end if;
      end case;
    end if;
  end process;
end rtl;


-- ============================================================
-- UART transmitter (ส่งหลายไบต์ต่อเนื่อง ไบต์ล่างสุดออกก่อน)
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity uart_frame_tx is
  generic (
    CLKS_PER_BIT : integer := 434;
    N            : integer := 5
  );
  port (
    clk   : in  std_logic;
    start : in  std_logic;
    data  : in  std_logic_vector(8 * N - 1 downto 0);
    busy  : out std_logic;
    tx    : out std_logic
  );
end uart_frame_tx;

architecture rtl of uart_frame_tx is
  signal sh   : std_logic_vector(10 * N - 1 downto 0) := (others => '1');
  signal cnt  : integer range 0 to CLKS_PER_BIT - 1 := 0;
  signal bits : integer range 0 to 10 * N := 0;
begin
  tx   <= sh(0);
  busy <= '1' when bits /= 0 else '0';

  process(clk)
  begin
    if rising_edge(clk) then
      if bits = 0 then
        if start = '1' then
          for i in 0 to N - 1 loop
            sh(10 * i + 9 downto 10 * i) <= '1' & data(8 * i + 7 downto 8 * i) & '0';
          end loop;
          bits <= 10 * N;
          cnt  <= 0;
        end if;
      elsif cnt = CLKS_PER_BIT - 1 then
        cnt  <= 0;
        sh   <= '1' & sh(10 * N - 1 downto 1);
        bits <= bits - 1;
      else
        cnt <= cnt + 1;
      end if;
    end if;
  end process;
end rtl;


-- ============================================================
-- Debounce
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity debounce is
  generic (
    STABLE_CLKS : integer   := 500_000;   -- 10 ms
    INIT        : std_logic := '0'
  );
  port (
    clk  : in  std_logic;
    din  : in  std_logic;
    dout : out std_logic
  );
end debounce;

architecture rtl of debounce is
  signal s1, s2 : std_logic := INIT;
  signal state  : std_logic := INIT;
  signal cnt    : integer range 0 to STABLE_CLKS := 0;
begin
  dout <= state;

  process(clk)
  begin
    if rising_edge(clk) then
      s1 <= din;
      s2 <= s1;
      if s2 = state then
        cnt <= 0;
      elsif cnt = STABLE_CLKS then
        state <= s2;
        cnt   <= 0;
      else
        cnt <= cnt + 1;
      end if;
    end if;
  end process;
end rtl;


-- ============================================================
-- ADC MCP3208 (อ่าน CH6 = LDR และ CH7 = LM35 สลับกัน แล้วเฉลี่ย)
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity mcp3208 is
  generic (
    HALF     : integer := 50;      -- SCK 500 kHz
    AVG_BITS : integer := 10       -- เฉลี่ย 2^10 = 1024 ครั้ง (อัปเดตราว 0.5 วินาที)
  );
  port (
    clk      : in  std_logic;
    adc_dout : in  std_logic;
    adc_sck  : out std_logic;
    adc_din  : out std_logic;
    adc_cs   : out std_logic;
    ldr      : out unsigned(11 downto 0);
    temp     : out unsigned(11 downto 0)
  );
end mcp3208;

architecture rtl of mcp3208 is
  signal div    : integer range 0 to HALF - 1 := 0;
  signal pause  : integer range 0 to 199 := 0;
  signal step   : integer range 0 to 37 := 0;
  signal cs     : std_logic := '1';
  signal sck    : std_logic := '0';
  signal din    : std_logic := '0';
  signal dout_s : std_logic := '0';
  signal ch     : std_logic_vector(2 downto 0) := "110";
  signal sh     : unsigned(11 downto 0) := (others => '0');

  signal sum_ldr, sum_temp : unsigned(11 + AVG_BITS downto 0) := (others => '0');
  signal cnt_ldr, cnt_temp : unsigned(AVG_BITS - 1 downto 0) := (others => '0');
  signal ldr_r, temp_r     : unsigned(11 downto 0) := (others => '0');   -- ค่าเริ่มต้น 0 จนเฉลี่ยครบรอบแรก
begin
  ldr     <= ldr_r;
  temp    <= temp_r;
  adc_cs  <= cs;
  adc_sck <= sck;
  adc_din <= din;

  -- clock 1-5: start, SGL, D2, D1, D0 / 6: sample / 7: null / 8-19: B11..B0
  process(clk)
    variable total : unsigned(11 + AVG_BITS downto 0);
  begin
    if rising_edge(clk) then
      dout_s <= adc_dout;

      if div = HALF - 1 then
        div <= 0;

        if cs = '1' then
          if pause = 199 then
            pause <= 0;
            cs    <= '0';
            din   <= '1';
            step  <= 0;
          else
            pause <= pause + 1;
          end if;

        elsif step mod 2 = 0 then
          sck <= '1';
          if step >= 14 then
            sh <= sh(10 downto 0) & dout_s;
          end if;
          step <= step + 1;

        else
          sck <= '0';
          case step is
            when 1      => din <= '1';
            when 3      => din <= ch(2);
            when 5      => din <= ch(1);
            when 7      => din <= ch(0);
            when others => din <= '0';
          end case;

          if step = 37 then
            cs <= '1';
            if ch = "110" then
              total   := sum_ldr + sh;
              cnt_ldr <= cnt_ldr + 1;
              if cnt_ldr = 2 ** AVG_BITS - 1 then
                ldr_r   <= total(11 + AVG_BITS downto AVG_BITS);
                sum_ldr <= (others => '0');
              else
                sum_ldr <= total;
              end if;
              ch <= "111";
            else
              total    := sum_temp + sh;
              cnt_temp <= cnt_temp + 1;
              if cnt_temp = 2 ** AVG_BITS - 1 then
                temp_r   <= total(11 + AVG_BITS downto AVG_BITS);
                sum_temp <= (others => '0');
              else
                sum_temp <= total;
              end if;
              ch <= "110";
            end if;
          else
            step <= step + 1;
          end if;
        end if;
      else
        div <= div + 1;
      end if;
    end if;
  end process;
end rtl;


-- ============================================================
-- 7-segment: อุณหภูมิแอร์ / ค่าดิบ LDR / อุณหภูมิห้อง
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sevenseg is
  port (
    clk   : in  std_logic;
    mode  : in  std_logic_vector(1 downto 0);    -- 00 แอร์, 01 LDR, 10 ห้อง
    ac    : in  unsigned(5 downto 0);            -- หน่วย 0.5 °C
    room  : in  unsigned(9 downto 0);            -- หน่วย 0.5 °C
    ldr   : in  unsigned(11 downto 0);           -- ค่าดิบ 0-4095
    seg   : out std_logic_vector(6 downto 0);    -- g f e d c b a (active low)
    dp    : out std_logic;
    digit : out std_logic_vector(3 downto 0)     -- active high, digit(3) = หลักซ้ายสุด
  );
end sevenseg;

architecture rtl of sevenseg is
  signal refresh : integer range 0 to 49_999 := 0;
  signal pos     : integer range 0 to 3 := 0;
  signal number  : unsigned(11 downto 0) := (others => '0');
  signal bcd     : unsigned(15 downto 0) := (others => '0');

  function pattern(n : integer) return std_logic_vector is
  begin
    case n is
      when 0      => return "0111111";
      when 1      => return "0000110";
      when 2      => return "1011011";
      when 3      => return "1001111";
      when 4      => return "1100110";
      when 5      => return "1101101";
      when 6      => return "1111101";
      when 7      => return "0000111";
      when 8      => return "1111111";
      when 9      => return "1101111";
      when 10     => return "0111001";   -- C
      when others => return "0000000";
    end case;
  end function;

  -- double dabble: เลขฐานสอง 12 บิต → BCD 4 หลัก
  function to_bcd(bin : unsigned(11 downto 0)) return unsigned is
    variable r : unsigned(15 downto 0) := (others => '0');
  begin
    for i in 11 downto 0 loop
      for d in 0 to 3 loop
        if r(4 * d + 3 downto 4 * d) > 4 then
          r(4 * d + 3 downto 4 * d) := r(4 * d + 3 downto 4 * d) + 3;
        end if;
      end loop;
      r := r(14 downto 0) & bin(i);
    end loop;
    return r;
  end function;
begin
  process(clk)
    variable n : integer range 0 to 15;
  begin
    if rising_edge(clk) then
      if refresh = 49_999 then
        refresh <= 0;
        if pos = 3 then pos <= 0; else pos <= pos + 1; end if;
      else
        refresh <= refresh + 1;
      end if;

      -- อุณหภูมิเก็บเป็นหน่วย 0.5 °C → คูณ 5 ได้หน่วย 0.1 °C เช่น 49 → 245 = 24.5
      if mode(0) = '1' then
        number <= ldr;
      elsif mode(1) = '1' then
        number <= resize(room * to_unsigned(5, 3), 12);
      else
        number <= resize(ac * to_unsigned(5, 3), 12);
      end if;
      bcd <= to_bcd(number);

      if mode(0) = '1' then                       -- ค่าดิบ 4 หลัก
        case pos is
          when 0 => n := to_integer(bcd(15 downto 12)); digit <= "1000";
          when 1 => n := to_integer(bcd(11 downto 8));  digit <= "0100";
          when 2 => n := to_integer(bcd(7 downto 4));   digit <= "0010";
          when 3 => n := to_integer(bcd(3 downto 0));   digit <= "0001";
        end case;
        seg <= not pattern(n);
        dp  <= '1';
      else                                        -- อุณหภูมิ เช่น 24.5C
        case pos is
          when 0 => n := to_integer(bcd(11 downto 8)); digit <= "1000";
          when 1 => n := to_integer(bcd(7 downto 4));  digit <= "0100";
          when 2 => n := to_integer(bcd(3 downto 0));  digit <= "0010";
          when 3 => n := 10;                           digit <= "0001";
        end case;
        seg <= not pattern(n);
        if pos = 1 then dp <= '0'; else dp <= '1'; end if;
      end if;
    end if;
  end process;
end rtl;

-- ============================================================
-- LCD 16x2 แสดงจำนวนที่นั่งว่าง / ไม่ว่าง
-- ไฟไหม้ (fire = '1'): บรรทัด 1 "!! FIRE ALARM !!" / บรรทัด 2 "EVACUATE NOW" กะพริบตาม blink
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity lcd16x2 is
  port (
    clk      : in  std_logic;
    free_cnt : in  unsigned(2 downto 0);   -- 0-7
    used_cnt : in  unsigned(2 downto 0);   -- 0-7
    fire     : in  std_logic;              -- 1 = ควันอันตราย แสดงข้อความไฟไหม้แทน
    blink    : in  std_logic;              -- จังหวะกะพริบบรรทัด 2 ตอนไฟไหม้
    lcd_rs   : out std_logic;
    lcd_en   : out std_logic;
    lcd_data : out std_logic_vector(7 downto 0)
  );
end lcd16x2;

architecture rtl of lcd16x2 is
  constant LINE1    : string(1 to 16) := "Free seats: 0   ";
  constant LINE2    : string(1 to 16) := "Used seats: 0   ";
  constant FIRE1    : string(1 to 16) := "!! FIRE ALARM !!";
  constant FIRE2    : string(1 to 16) := "  EVACUATE NOW  ";
  constant POWER_UP : integer := 2_500_000;   -- 50 ms
  constant T_LONG   : integer := 250_000;     -- 5 ms
  constant T_SHORT  : integer := 2_500;       -- 50 us

  signal boot : integer range 0 to POWER_UP := 0;
  signal cnt  : integer range 0 to T_LONG := 0;
  signal idx  : integer range 0 to 39 := 0;

  function ascii(c : character) return std_logic_vector is
  begin
    return std_logic_vector(to_unsigned(character'pos(c), 8));
  end function;

  function digit_char(n : unsigned) return std_logic_vector is
  begin
    return std_logic_vector(to_unsigned(48 + to_integer(n), 8));
  end function;
begin
  -- idx 0-5: คำสั่งเริ่มต้น / 6: บรรทัด 1 / 7-22: ตัวอักษร / 23: บรรทัด 2 / 24-39: ตัวอักษร
  process(clk)
    variable rs     : std_logic;
    variable d      : std_logic_vector(7 downto 0);
    variable period : integer range 0 to T_LONG;
  begin
    if rising_edge(clk) then
      if boot < POWER_UP then
        boot   <= boot + 1;
        lcd_en <= '0';
      else
        rs := '0';
        case idx is
          when 0 | 1 | 2          => d := x"38";
          when 3                  => d := x"0C";
          when 4                  => d := x"01";
          when 5                  => d := x"06";
          when 6                  => d := x"80";
          when 23                 => d := x"C0";
          when 7 to 22 =>                                   -- บรรทัด 1
            rs := '1';
            if fire = '1' then
              d := ascii(FIRE1(idx - 6));
            elsif idx = 19 then
              d := digit_char(free_cnt);
            else
              d := ascii(LINE1(idx - 6));
            end if;
          when others =>                                    -- บรรทัด 2 (idx 24-39)
            rs := '1';
            if fire = '1' then
              if blink = '1' then d := ascii(FIRE2(idx - 23)); else d := ascii(' '); end if;
            elsif idx = 36 then
              d := digit_char(used_cnt);
            else
              d := ascii(LINE2(idx - 23));
            end if;
        end case;

        if idx <= 5 then period := T_LONG; else period := T_SHORT; end if;

        lcd_rs   <= rs;
        lcd_data <= d;
        if cnt = 50 then
          lcd_en <= '1';
        elsif cnt = 100 then
          lcd_en <= '0';
        end if;

        if cnt = period then
          cnt <= 0;
          if idx = 39 then idx <= 6; else idx <= idx + 1; end if;
        else
          cnt <= cnt + 1;
        end if;
      end if;
    end if;
  end process;
end rtl;


-- ============================================================
-- เครื่องเล่นเพลงสั้นทาง buzzer (piezo เรโซแนนซ์ 3.8 kHz คู่มือหน้า 16)
-- start = '1' หนึ่ง clock -> เล่นเพลงจนจบ (กด start ระหว่างเล่น = เริ่มใหม่)
-- เพลง: ท่อนเปิด Super Mario Bros. จังหวะเร็ว โน้ตละ 125 ms (E E - E - C E G - G ต่ำ) ยาวประมาณ 2 วินาที
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity melody is
  port (
    clk   : in  std_logic;
    start : in  std_logic;
    tone  : out std_logic;
    busy  : out std_logic
  );
end melody;

architecture rtl of melody is
  constant MS  : integer := 50_000;   -- 1 ms ที่ 50 MHz
  constant GAP : integer := 25;       -- เงียบท้ายโน้ต 25 ms ให้โน้ตซ้ำแยกกันได้ยิน

  -- half = จำนวน clock ครึ่งคาบ = 50 MHz / (2 x ความถี่) / 0 = พัก
  constant R  : integer := 0;         -- พัก
  constant G5 : integer := 31_888;    --  784.0 Hz
  constant C6 : integer := 23_889;    -- 1046.5 Hz
  constant D6 : integer := 21_282;    -- 1174.7 Hz
  constant E6 : integer := 18_961;    -- 1318.5 Hz
  constant F6 : integer := 17_897;    -- 1396.9 Hz
  constant G6 : integer := 15_944;    -- 1568.0 Hz
  constant A6 : integer := 14_205;    -- 1760.0 Hz

  type note_t is record
    half : integer range 0 to 32_767;
    ms   : integer range 1 to 2_000;
  end record;
  type song_t is array (natural range <>) of note_t;
  constant SONG : song_t := (
    (E6, 125), (E6, 125), (R, 125), (E6, 125),
    (R, 125), (C6, 125), (E6, 250), (G6, 250),
    (R, 250), (G5, 250)
  );

  signal playing : std_logic := '0';
  signal idx     : integer range 0 to SONG'high := 0;
  signal ms_cnt  : integer range 0 to MS - 1 := 0;
  signal ms_left : integer range 0 to 2_000 := 0;
  signal tcnt    : integer range 0 to 32_767 := 0;
  signal sq      : std_logic := '0';
begin
  busy <= playing;
  tone <= sq when playing = '1' and ms_left > GAP and SONG(idx).half /= 0 else '0';

  process(clk)
  begin
    if rising_edge(clk) then
      if start = '1' then
        playing <= '1';
        idx     <= 0;
        ms_cnt  <= 0;
        ms_left <= SONG(0).ms;
        tcnt    <= 0;
        sq      <= '0';
      elsif playing = '1' then
        -- สร้างคลื่นสี่เหลี่ยมตามความถี่ของโน้ต
        if tcnt >= SONG(idx).half - 1 then
          tcnt <= 0;
          sq   <= not sq;
        else
          tcnt <= tcnt + 1;
        end if;

        -- นับเวลาโน้ตทีละ 1 ms
        if ms_cnt = MS - 1 then
          ms_cnt <= 0;
          if ms_left <= 1 then
            if idx = SONG'high then
              playing <= '0';
            else
              idx     <= idx + 1;
              ms_left <= SONG(idx + 1).ms;
              tcnt    <= 0;
            end if;
          else
            ms_left <= ms_left - 1;
          end if;
        else
          ms_cnt <= ms_cnt + 1;
        end if;
      end if;
    end if;
  end process;
end rtl;


-- ============================================================
-- Top: บอร์ด 1
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity board1_top is
  port (
    clk       : in  std_logic;
    usb_rx    : in  std_logic;
    usb_tx    : out std_logic;
    esp_tx    : in  std_logic;
    esp_rx    : out std_logic;
    btn_light : in  std_logic;
    btn_fan   : in  std_logic;
    btn_mode  : in  std_logic;                      -- SW18 ปุ่มกด: สลับโหมดไฟ Auto / Manual
    sw_seat   : in  std_logic_vector(2 downto 0);   -- SW2,SW4,SW5 = เลขฐานสอง จำนวนโต๊ะที่มีคน 0-7 (bit0 = SW2)
    sw_view   : in  std_logic_vector(1 downto 0);   -- เลือกสิ่งที่แสดงบน 7-segment
    sw_buzz   : in  std_logic;                      -- SW9 ทดสอบ buzzer
    sw_prog   : in  std_logic;                      -- SW8: 1 = คอมคุย ESP ตรง (อัปโหลดโปรแกรม ESP)
    smoke_do  : in  std_logic;                      -- MQ-2 DO, LOW = เจอควัน
    adc_dout  : in  std_logic;
    adc_sck   : out std_logic;
    adc_din   : out std_logic;
    adc_cs    : out std_logic;
    room_led  : out std_logic;                      -- ไฟห้อง LED ขาว 1 ดวง
    fan       : out std_logic;
    buzzer    : out std_logic;
    seg       : out std_logic_vector(6 downto 0);
    dp        : out std_logic;
    digit     : out std_logic_vector(3 downto 0);
    lcd_rs    : out std_logic;
    lcd_en    : out std_logic;
    lcd_data  : out std_logic_vector(7 downto 0);
    link_tx   : out std_logic                       -- ไปบอร์ด 2
  );
end board1_top;

architecture rtl of board1_top is
  constant CLKS_PER_BIT : integer := 434;
  constant LDR_DARK     : integer := 2100;   -- ต่ำกว่านี้ = มืด
  constant LDR_BRIGHT   : integer := 2300;   -- สูงกว่านี้ = สว่าง
  constant ROOM_LOW     : integer := 46;     -- 23.0 °C (หน่วย 0.5 °C)
  constant ROOM_HIGH    : integer := 54;     -- 27.0 °C
  constant AC_MIN       : integer := 32;     -- 16.0 °C
  constant AC_MAX       : integer := 54;     -- 27.0 °C
  -- LM35 10 mV/°C, ADC 12 บิต Vref 3.3 V → หน่วย 0.5 °C = code x 165 / 1024 ตามทฤษฎี
  -- โค้ดเดิมใช้ 190 (อาจปรับเทียบกับเทอร์โมมิเตอร์แล้ว UNVERIFIED) ตรวจบนบอร์ดด้วย sw_view = 10
  constant TEMP_SCALE   : integer := 210;
  constant BTN_CLKS     : integer := 1_000_000;  -- กันเด้งปุ่ม 20 ms
  constant SEAT_TOTAL   : integer := 7;          -- จำนวนโต๊ะทั้งหมด (sw_seat นับได้ 0-7)

  signal light_btn, light_btn_d : std_logic := '0';
  signal fan_btn, fan_btn_d     : std_logic := '0';
  signal mode_btn, mode_btn_d   : std_logic := '0';
  signal song_start, song_tone  : std_logic := '0';
  signal alarm                  : std_logic;
  signal seat      : std_logic_vector(2 downto 0);
  signal smoke_raw : std_logic;

  signal ldr_code, temp_code : unsigned(11 downto 0) := (others => '0');
  signal room2 : unsigned(9 downto 0);

  signal manual, dark, light_manual, light_on, fan_on, danger : std_logic := '0';

  type ac_state_t is (HOLD, COOL, WARM);
  signal ac_state : ac_state_t := HOLD;
  signal ac       : unsigned(5 downto 0) := to_unsigned(50, 6);
  signal tick_2s  : integer range 0 to 99_999_999 := 0;

  signal used_cnt, free_cnt : unsigned(2 downto 0) := (others => '0');
  signal loc : std_logic_vector(7 downto 0) := (others => '0');

  signal beep_cnt : integer range 0 to 12_499_999 := 0;  -- 0.25 s
  signal beep     : std_logic := '0';

  signal rx_byte  : std_logic_vector(7 downto 0);
  signal rx_valid : std_logic;
  type frame_t is (WAIT_HDR, WAIT_CMD, WAIT_ARG, WAIT_CHK);
  signal frame : frame_t := WAIT_HDR;
  signal cmd, arg : std_logic_vector(7 downto 0) := (others => '0');

  signal flags, ac_byte, status_chk : std_logic_vector(7 downto 0);
  signal status_frame : std_logic_vector(39 downto 0);
  signal status_send  : std_logic := '0';
  signal status_req   : std_logic := '0';   -- ค้างคำขอไว้จนตัวส่งว่าง
  signal status_busy  : std_logic;
  signal fpga_tx      : std_logic;

  signal link_frame : std_logic_vector(23 downto 0);
  signal link_tick  : integer range 0 to 2_499_999 := 0;   -- 50 ms
  signal link_send  : std_logic := '0';
begin

  db_light : entity work.debounce generic map (STABLE_CLKS => BTN_CLKS)
                                  port map (clk => clk, din => btn_light, dout => light_btn);
  db_fan   : entity work.debounce generic map (STABLE_CLKS => BTN_CLKS)
                                  port map (clk => clk, din => btn_fan,   dout => fan_btn);
  db_mode  : entity work.debounce generic map (STABLE_CLKS => BTN_CLKS)
                                  port map (clk => clk, din => btn_mode,  dout => mode_btn);

  song : entity work.melody
    port map (clk => clk, start => song_start, tone => song_tone, busy => open);
  db_smoke : entity work.debounce generic map (INIT => '1')
                                  port map (clk => clk, din => smoke_do,  dout => smoke_raw);
  gen_seat : for i in 0 to 2 generate
    db_seat : entity work.debounce port map (clk => clk, din => sw_seat(i), dout => seat(i));
  end generate;

  adc : entity work.mcp3208
    port map (clk => clk, adc_dout => adc_dout, adc_sck => adc_sck,
              adc_din => adc_din, adc_cs => adc_cs, ldr => ldr_code, temp => temp_code);

  display : entity work.sevenseg
    port map (clk => clk, mode => sw_view, ac => ac, room => room2, ldr => ldr_code,
              seg => seg, dp => dp, digit => digit);

  lcd : entity work.lcd16x2
    port map (clk => clk, free_cnt => free_cnt, used_cnt => used_cnt, fire => danger, blink => beep,
              lcd_rs => lcd_rs, lcd_en => lcd_en, lcd_data => lcd_data);

  esp_in : entity work.uart_rx generic map (CLKS_PER_BIT => CLKS_PER_BIT)
    port map (clk => clk, rx => esp_tx, data => rx_byte, valid => rx_valid);

  esp_out : entity work.uart_frame_tx generic map (CLKS_PER_BIT => CLKS_PER_BIT, N => 5)
    port map (clk => clk, start => status_send, data => status_frame, busy => status_busy, tx => fpga_tx);

  board2_out : entity work.uart_frame_tx generic map (CLKS_PER_BIT => CLKS_PER_BIT, N => 3)
    port map (clk => clk, start => link_send, data => link_frame, busy => open, tx => link_tx);

  -- อุณหภูมิห้องหน่วย 0.5 °C = code x TEMP_SCALE / 1024
  room2 <= resize(shift_right(temp_code * to_unsigned(TEMP_SCALE, 8), 10), 10);

  light_on <= light_manual when manual = '1' else dark;
  danger   <= not smoke_raw;

  flags        <= '0' & seat & danger & fan_on & manual & light_on;   -- bit4-6 = จำนวนโต๊ะที่มีคน 0-7
  ac_byte      <= "00" & std_logic_vector(ac);
  status_chk   <= not (flags xor ac_byte xor loc);
  status_frame <= status_chk & loc & ac_byte & flags & x"5A";
  link_frame   <= (not loc) & loc & x"A5";

  room_led <= light_on;
  fan      <= fan_on;
  -- buzzer แบบ active (มีวงจรสั่นในตัว): จ่าย '1' ค้าง = ดัง / ดัง 0.25 s เงียบ 0.25 s
  -- เสียงเตือนควัน/ทดสอบ มาก่อนเพลง / ไม่มีเตือน = เล่นเพลงตอนกด Follow
  alarm  <= danger or sw_buzz;
  buzzer <= beep when alarm = '1' else song_tone;

  usb_tx <= esp_tx;                 -- คอมเห็นสิ่งที่ ESP ส่ง (ใช้ตอนอัปโหลด)
  -- sw_prog = 1: ESP รับจากคอม (อัปโหลด) / 0: ESP รับจาก FPGA อย่างเดียว ขา usb_rx ลอยก็ไม่กวน
  esp_rx <= usb_rx when sw_prog = '1' else fpga_tx;

  process(clk)
  begin
    if rising_edge(clk) then
      status_send <= '0';
      link_send   <= '0';
      song_start  <= '0';

      -- ส่งสถานะเมื่อตัวส่งว่าง (ถ้ากำลังส่งเฟรมก่อนหน้า คำขอจะรอ ไม่หาย)
      if status_req = '1' and status_busy = '0' and status_send = '0' then
        status_send <= '1';
        status_req  <= '0';
      end if;

      -- ไฟห้อง: Auto ตาม LDR / Manual ตามปุ่มบนบอร์ดหรือเว็บ
      if ldr_code < LDR_DARK then
        dark <= '1';
      elsif ldr_code > LDR_BRIGHT then
        dark <= '0';
      end if;

      light_btn_d <= light_btn;
      if light_btn = '1' and light_btn_d = '0' and manual = '1' then
        light_manual <= not light_manual;
      end if;

      -- โหมดไฟ: ปุ่ม SW18 สลับ Auto / Manual (เข้า Manual แล้วไฟคงสถานะเดิมของ Auto เหมือนสั่งจากเว็บ)
      mode_btn_d <= mode_btn;
      if mode_btn = '1' and mode_btn_d = '0' then
        if manual = '0' then
          light_manual <= dark;
        end if;
        manual <= not manual;
      end if;

      -- พัดลม: ปุ่มบนบอร์ดหรือเว็บ
      fan_btn_d <= fan_btn;
      if fan_btn = '1' and fan_btn_d = '0' then
        fan_on <= not fan_on;
      end if;

      -- แอร์: ปรับอัตโนมัติทุก 2 วินาที
      if tick_2s = 99_999_999 then
        tick_2s <= 0;
        if room2 > ROOM_HIGH then
          ac_state <= COOL;
          if ac > AC_MIN then ac <= ac - 1; end if;
        elsif room2 < ROOM_LOW then
          ac_state <= WARM;
          if ac < AC_MAX then ac <= ac + 1; end if;
        else
          ac_state <= HOLD;
        end if;
      else
        tick_2s <= tick_2s + 1;
      end if;

      -- โต๊ะ: sw_seat เป็นเลขฐานสอง 0-7 = จำนวนโต๊ะที่มีคนนั่ง จากทั้งหมด SEAT_TOTAL โต๊ะ
      used_cnt <= unsigned(seat);
      free_cnt <= to_unsigned(SEAT_TOTAL, 3) - unsigned(seat);

      -- buzzer
      if beep_cnt = 12_499_999 then
        beep_cnt <= 0;
        beep     <= not beep;
      else
        beep_cnt <= beep_cnt + 1;
      end if;

      -- คำสั่งจาก ESP
      if rx_valid = '1' then
        case frame is
          when WAIT_HDR =>
            if rx_byte = x"A5" then frame <= WAIT_CMD; end if;
          when WAIT_CMD =>
            cmd   <= rx_byte;
            frame <= WAIT_ARG;
          when WAIT_ARG =>
            arg   <= rx_byte;
            frame <= WAIT_CHK;
          when WAIT_CHK =>
            if rx_byte = not (cmd xor arg) then
              case cmd is
                when x"01" =>                           -- โหมดไฟ
                  if arg(0) = '1' and manual = '0' then
                    light_manual <= dark;
                  end if;
                  manual <= arg(0);
                when x"02" =>                           -- หาหนังสือ
                  loc <= arg;
                  if arg /= x"00" then                  -- Follow (ไม่ใช่ยกเลิก) = เล่นเพลง
                    song_start <= '1';
                  end if;
                when x"03" =>                           -- ไฟห้อง (Manual เท่านั้น)
                  if manual = '1' then
                    light_manual <= arg(0);
                  end if;
                when x"04" =>                           -- พัดลม
                  fan_on <= arg(0);
                when others =>
                  null;
              end case;
              status_req <= '1';
            end if;
            frame <= WAIT_HDR;
        end case;
      end if;

      -- ส่งตำแหน่งหนังสือให้บอร์ด 2 ทุก 50 ms
      if link_tick = 2_499_999 then
        link_tick <= 0;
        link_send <= '1';
      else
        link_tick <= link_tick + 1;
      end if;
    end if;
  end process;

end rtl;