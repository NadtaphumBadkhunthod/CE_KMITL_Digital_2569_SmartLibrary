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
-- 7-segment: ตำแหน่งหนังสือ เช่น "1-2 " = โซน 1 ชั้น 2 / ไม่มีคำสั่ง = "----"
-- จุดหลักขวาสุดสลับทุกเฟรมที่ถูกต้อง (ไฟบอกว่าสายคุยกันอยู่)
-- debug = 1: แสดง "E" + จำนวนเฟรมเสีย 3 หลัก
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sevenseg_loc is
  port (
    clk   : in  std_logic;
    valid : in  std_logic;                       -- 1 = มีตำแหน่งที่ถูกต้อง
    zone  : in  unsigned(3 downto 0);
    shelf : in  unsigned(3 downto 0);
    beat  : in  std_logic;                       -- สลับทุกเฟรมที่ถูกต้อง
    debug : in  std_logic;
    err   : in  unsigned(11 downto 0);           -- จำนวนเฟรมเสียแบบ BCD 3 หลัก
    seg   : out std_logic_vector(6 downto 0);    -- g f e d c b a (active low)
    dp    : out std_logic;
    digit : out std_logic_vector(3 downto 0)     -- active high, digit(3) = หลักซ้ายสุด
  );
end sevenseg_loc;

architecture rtl of sevenseg_loc is
  constant DASH  : integer := 10;
  constant BLANK : integer := 11;
  constant LET_E : integer := 12;

  signal refresh : integer range 0 to 49_999 := 0;
  signal pos     : integer range 0 to 3 := 0;

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
      when DASH   => return "1000000";   -- -
      when LET_E  => return "1111001";   -- E
      when others => return "0000000";   -- ว่าง
    end case;
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

      if debug = '1' then
        case pos is
          when 0 => digit <= "1000"; n := LET_E;
          when 1 => digit <= "0100"; n := to_integer(err(11 downto 8));
          when 2 => digit <= "0010"; n := to_integer(err(7 downto 4));
          when 3 => digit <= "0001"; n := to_integer(err(3 downto 0));
        end case;
      else
        case pos is
          when 0 => digit <= "1000";
                    if valid = '1' then n := to_integer(zone); else n := DASH; end if;
          when 1 => digit <= "0100"; n := DASH;
          when 2 => digit <= "0010";
                    if valid = '1' then n := to_integer(shelf); else n := DASH; end if;
          when 3 => digit <= "0001";
                    if valid = '1' then n := BLANK; else n := DASH; end if;
        end case;
      end if;
      seg <= not pattern(n);
      if pos = 3 then dp <= not beat; else dp <= '1'; end if;   -- dp active low
    end if;
  end process;
end rtl;

-- ============================================================
-- Top: บอร์ด 2 (ไฟโซน 3 ดวง + ไฟชั้นหนังสือ 3 โซน x 3 ชั้น = 12 ดวง)
-- รับเฟรม A5, loc, not loc จากบอร์ด 1 ทุก 50 ms / loc = โซน(7:4) & ชั้น(3:0), 1 = แดง 2 = น้ำเงิน 3 = เขียว
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity board2_top is
  port (
    clk       : in  std_logic;
    link_rx   : in  std_logic;                      -- จาก link_tx ของบอร์ด 1
    sw_debug  : in  std_logic;                      -- SW2: 1 = แสดงจำนวนเฟรมเสีย
    zone_led  : out std_logic_vector(2 downto 0);   -- ไฟโซน: zone_led(โซน - 1), active high
    shelf_led : out std_logic_vector(8 downto 0);   -- ไฟชั้น: (โซน - 1) x 3 + (ชั้น - 1), active high
    seg       : out std_logic_vector(6 downto 0);
    dp        : out std_logic;
    digit     : out std_logic_vector(3 downto 0)
  );
end board2_top;

architecture rtl of board2_top is
  constant CLKS_PER_BIT : integer := 434;
  constant TIMEOUT      : integer := 24_999_999;   -- 500 ms ไม่มีเฟรม = ดับไฟ

  signal rx_byte  : std_logic_vector(7 downto 0);
  signal rx_valid : std_logic;

  type frame_t is (WAIT_HDR, WAIT_LOC, WAIT_CHK);
  signal frame : frame_t := WAIT_HDR;
  signal loc_rx : std_logic_vector(7 downto 0) := (others => '0');
  signal loc    : std_logic_vector(7 downto 0) := (others => '0');
  signal idle   : integer range 0 to TIMEOUT := 0;

  signal zone, shelf : unsigned(3 downto 0);
  signal loc_ok      : std_logic;

  signal beat    : std_logic := '0';                         -- สลับทุกเฟรมที่ถูกต้อง
  signal err_bcd : unsigned(11 downto 0) := (others => '0'); -- เฟรมเสีย 000-999 (หยุดที่ 999)
begin

  link_in : entity work.uart_rx generic map (CLKS_PER_BIT => CLKS_PER_BIT)
    port map (clk => clk, rx => link_rx, data => rx_byte, valid => rx_valid);

  display : entity work.sevenseg_loc
    port map (clk => clk, valid => loc_ok, zone => zone, shelf => shelf,
              beat => beat, debug => sw_debug, err => err_bcd,
              seg => seg, dp => dp, digit => digit);

  zone   <= unsigned(loc(7 downto 4));
  shelf  <= unsigned(loc(3 downto 0));
  loc_ok <= '1' when zone >= 1 and zone <= 3 and shelf >= 1 and shelf <= 3 else '0';

  -- decoder ไฟโซน: เลือก 1 ดวงจาก 3 ดวง ค่าอื่นดับหมด
  with loc select zone_led <=
    "001" when x"11" | x"12" | x"13",   -- โซน 1
    "010" when x"21" | x"22" | x"23",   -- โซน 2
    "100" when x"31" | x"32" | x"33",   -- โซน 3
    "000" when others;

  -- decoder ไฟชั้น: เลือก LED 1 ดวงจาก 9 ดวง ค่าอื่นดับหมด
  with loc select shelf_led <=
    "000000001" when x"11",   -- โซนแดง    ชั้นแดง
    "000000010" when x"12",   -- โซนแดง    ชั้นน้ำเงิน
    "000000100" when x"13",   -- โซนแดง    ชั้นเขียว
    "000001000" when x"21",   -- โซนน้ำเงิน ชั้นแดง
    "000010000" when x"22",
    "000100000" when x"23",
    "001000000" when x"31",   -- โซนเขียว   ชั้นแดง
    "010000000" when x"32",
    "100000000" when x"33",
    "000000000" when others;

  process(clk)
    variable bad : boolean;
  begin
    if rising_edge(clk) then
      if idle = TIMEOUT then
        loc <= (others => '0');
      else
        idle <= idle + 1;
      end if;

      bad := false;
      if rx_valid = '1' then
        case frame is
          when WAIT_HDR =>
            if rx_byte = x"A5" then
              frame <= WAIT_LOC;
            else
              bad := true;                  -- ไบต์แปลกปลอมนอกเฟรม
            end if;
          when WAIT_LOC =>
            loc_rx <= rx_byte;
            frame  <= WAIT_CHK;
          when WAIT_CHK =>
            if rx_byte = not loc_rx then
              loc  <= loc_rx;
              idle <= 0;
              beat <= not beat;
              frame <= WAIT_HDR;
            elsif rx_byte = x"A5" then
              bad := true;
              frame <= WAIT_LOC;            -- เฟรมเสีย แต่ไบต์นี้อาจเป็นหัวเฟรมใหม่
            else
              bad := true;
              frame <= WAIT_HDR;
            end if;
        end case;
      end if;

      -- นับเฟรมเสียเป็น BCD 3 หลัก หยุดที่ 999
      if bad and err_bcd /= x"999" then
        if err_bcd(3 downto 0) /= 9 then
          err_bcd(3 downto 0) <= err_bcd(3 downto 0) + 1;
        else
          err_bcd(3 downto 0) <= (others => '0');
          if err_bcd(7 downto 4) /= 9 then
            err_bcd(7 downto 4) <= err_bcd(7 downto 4) + 1;
          else
            err_bcd(7 downto 4)  <= (others => '0');
            err_bcd(11 downto 8) <= err_bcd(11 downto 8) + 1;
          end if;
        end if;
      end if;
    end if;
  end process;

end rtl;
