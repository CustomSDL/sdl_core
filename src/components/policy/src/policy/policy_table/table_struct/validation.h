#ifndef POLICY_TABLE_POLICY_TABLE_VALIDATION_H_
#define POLICY_TABLE_POLICY_TABLE_VALIDATION_H_

#include "types.h"
#include "utils/macro.h"

namespace rpc {
namespace policy_table_interface_base {

class AccessModulesValidator {
 public:
  bool Validate(const AccessModules& modules) const;
 private:
  typedef const std::pair<const std::string*, const std::string*> Iterators;
  inline bool ValidateRemoteRpcs(ModuleType module,
                                 const RemoteRpcs& rpcs) const;
  inline bool ValidateParameters(ModuleType module,
                                 const Strings& rpcs) const;
  inline Iterators GetModuleParameters(ModuleType module) const;
  FRIEND_TEST(InteriorZoneValidationTest, ValidateAllow);
};

}  // namespace policy_table_interface_base
}  // namespace rpc

#endif  // POLICY_TABLE_POLICY_TABLE_VALIDATION_H_
