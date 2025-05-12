# UHL State Tree

**UHL-StateTree** - Unreal plugin providing tools for **StateTree AI**, similar to UHL BehaviorTrees nodes. Simplify integration of StateTree into your project with ready-to-use components - tasks/evaluators/conditions/considirations and blueprint libraries.

Support: `UE5.5 (main)`

---

## ✨ Features

- cooldowns by tag
- InRange and other nodes similar to [UnrealHelperLibrary](https://github.com/Ciberusps/unreal-helper-library) BehaviorTree nodes

---

## 🛠️ Install

#### From source (recommended):

1. Add plugin as a git submodule in your project's `Plugins` folder:

   ```bash
   // install of UnrealHelperLibrary required
   git submodule add https://github.com/Ciberusps/unreal-helper-library.git ./Plugins/UnrealHelperLibrary
   git submodule add https://github.com/Ciberusps/uhl-state-tree.git ./Plugins/UHLStateTree
   ```

2. (Optional) For C++ projects, modify your `<ProjectName>.Build.cs`:

   ```csharp
   // <ProjectName>.Build.cs
   PublicDependencyModuleNames.AddRange(new string[] {
       "UHLStateTree",
   });
   ```

> **NOTE**: To target a specific engine version (e.g. UE5.4), specify branch:
> `git submodule add -b UE5.4 https://github.com/Ciberusps/uhl-state-tree.git ./Plugins/UHLStateTree`

---

## ⬆️ Update

From source:

- `git submodule update --remote` to update plugin(s) from source

---

## 📦 Modules

UHL-StateTree consists of a single module:

- **UHLStateTree** – core runtime and editor support for StateTree integration.

---

## 📖 Documentation

> - Components
>   - [UHLStateTreeAIComponent](#UUHLStateTreeAIComponent)
> - Conditions
>   - [StateTreeTagCooldownCondition](#StateTreeTagCooldownCondition)
> - Tasks
>   - [StateTreeSetCooldownTask](#StateTreeSetCooldownTask)

### `UUHLStateTreeComponent`

Attach this component to an `AAIController` in Blueprint or C++.

## 🤝 Contributing

Contributions welcome! Feel free to open issues or pull requests.

1. Fork repository
2. Create feature branch (`git checkout -b feature/my-feature`)
3. Commit changes (`git commit -am 'Add feature'`)
4. Push branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License**. See [LICENSE](LICENSE) for details.
