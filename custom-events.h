class BpmDjEvent: public QCustomEvent
{
 public:
  BpmDjEvent() : QCustomEvent(BpmDjCustom)
    {
    }
  virtual void run(SongSelectorLogic * ss) = 0;
}
