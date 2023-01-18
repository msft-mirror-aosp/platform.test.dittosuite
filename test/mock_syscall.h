#include <gmock/gmock.h>

#include <ditto/syscall.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::ReturnArg;

class MockSyscall : public dittosuite::SyscallInterface {
 public:
  static constexpr int kDefaultFileSize = 4096;
  static constexpr int kDefaultFileDescriptor = 4;

  // Set default returns for each syscall (mostly return 0 to indicate a successful call)
  MockSyscall() {
    ON_CALL(*this, Close(_)).WillByDefault(Return(0));
    ON_CALL(*this, CloseDir(_)).WillByDefault(Return(0));
    ON_CALL(*this, FAdvise(_, _, _, _)).WillByDefault(Return(0));
    ON_CALL(*this, FAllocate(_, _, _, _)).WillByDefault(Return(0));
    ON_CALL(*this, FTruncate(_, _)).WillByDefault(Return(0));
    ON_CALL(*this, FStat(_, _)).WillByDefault(Invoke([](int, struct stat64* buf) {
      buf->st_size = kDefaultFileSize;
      return 0;
    }));
    ON_CALL(*this, FSync(_)).WillByDefault(Return(0));
    ON_CALL(*this, Open(_, _)).WillByDefault(Return(kDefaultFileDescriptor));
    ON_CALL(*this, Read(_, _, _, _)).WillByDefault(ReturnArg<2>());
    ON_CALL(*this, ReadLink(_, _, _)).WillByDefault(ReturnArg<2>());
    ON_CALL(*this, Unlink(_)).WillByDefault(Return(0));
    ON_CALL(*this, Write(_, _, _, _)).WillByDefault(ReturnArg<2>());
  }

  MOCK_METHOD(int, Close, (int fd), (override));
  MOCK_METHOD(int, CloseDir, (DIR * dirp), (override));
  MOCK_METHOD(int, FAdvise, (int fd, int64_t offset, int64_t len, int advice), (override));
  MOCK_METHOD(int, FAllocate, (int fd, int mode, int64_t offset, int64_t len), (override));
  MOCK_METHOD(int, FTruncate, (int fd, int64_t length), (override));
  MOCK_METHOD(int, FStat, (int filedes, struct stat64* buf), (override));
  MOCK_METHOD(int, FSync, (int fd), (override));
  MOCK_METHOD(int, Open, (const std::string& path_name, bool create), (override));
  MOCK_METHOD(DIR*, OpenDir, (const std::string& name), (override));
  MOCK_METHOD(int64_t, Read, (int fd, char* buf, int64_t count, int64_t offset), (override));
  MOCK_METHOD(struct dirent*, ReadDir, (DIR * dirp), (override));
  MOCK_METHOD(int64_t, ReadLink, (const std::string& path_name, char* buf, int64_t bufsiz),
              (override));
  MOCK_METHOD(int, Unlink, (const std::string& path_name), (override));
  MOCK_METHOD(int64_t, Write, (int fd, char* buf, int64_t count, int64_t offset), (override));
};
